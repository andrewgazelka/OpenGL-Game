#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdio>
#include <iostream>

// Shader macro
#define GLSL(src) "#version 150 core\n" #src

//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define GLM_FORCE_RADIANS

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "utils.h"
#include "State.h"
#include "Scene.h"
#include "parse/MapParser.h"

using namespace std;

bool saveOutput = false;
float timePast = 0;

const float FOV_Y = 3.14f / 4;
const float STRAFE_SPEED = 0.07;
const float LOOK_SPEED = 0.03;

void handleKeyPress(State &state, int code) {
    switch (code) {
        case SDLK_q:
        case SDLK_ESCAPE:
            state.quit = true;
            break;
        case SDLK_f:
            state.fullscreen = !state.fullscreen;
        default:
            break;
    }
}

void handleKeyHold(State &state, int code) {
    auto &movement = state.movement;
    switch (code) {
        case SDLK_a:
            movement.look = Look::LEFT;
            break;
        case SDLK_d:
            movement.look = Look::RIGHT;
            break;
        case SDLK_w:
            movement.strafe = Strafe::FORWARD;
            break;
        case SDLK_s:
            movement.strafe = Strafe::BACKWARD;
            break;
        default:
            break;
    }
}

bool fullscreen = false;
int screen_width = 800;
int screen_height = 600;

char window_title[] = "My OpenGL Program";

float avg_render_time = 0;

int main(int argc, char *argv[]) {

    Map map = MapParser::parseMap("maps/test.txt");

    State state{
            .camPosition = glm::vec3(3.f, 0.f, 0.f)
    };

    Utils::SDLInit();

    //Create cubeTexturedModel window (offsetx, offsety, width, height, flags)
    SDL_Window *window = SDL_CreateWindow(window_title, 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    float aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

    //Create cubeTexturedModel context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    Utils::loadGlad();

    auto woodTexture = Utils::loadBMP("textures/wood.bmp");
    auto brickTexture = Utils::loadBMP("textures/brick.bmp");

    Model cubeModel = Utils::loadModel("models/cube.txt");
    Model model2 = Utils::loadModel("models/knot.txt");


    // combine into one array and change pointers of other models
    Model combined = Model::combine({&cubeModel, &model2});

    std::cout << "cubeModel: " << cubeModel << std::endl;
    std::cout << "model2: "<< model2 << std::endl;
    std::cout << "combined: "<< combined << std::endl;

    //Build cubeTexturedModel Vertex Array Object (VAO) to store mapping of shader attributse to VBO
    GLuint vao;
    glGenVertexArrays(1, &vao); //Create cubeTexturedModel VAO
    glBindVertexArray(vao); //Bind the above created VAO to the current context

    //Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo[1];
    glGenBuffers(1, vbo);  //Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at cubeTexturedModel time)
    glBufferData(GL_ARRAY_BUFFER, combined.GetNumberLines() * sizeof(float), combined.data.data(), GL_STATIC_DRAW); //upload vertices to vbo

    auto texturedShader = Utils::InitShader("shaders/textured-Vertex.glsl", "shaders/textured-Fragment.glsl");

    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    //GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);

    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));

    GLint uniView = glGetUniformLocation(texturedShader, "view");
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");

    glBindVertexArray(0); //Unbind the VAO in case we want to create cubeTexturedModel new one

    glEnable(GL_DEPTH_TEST);

    TexturedModel cubeTexturedModel = {
            .model = cubeModel,
            .textureId = 2,
    };

    TextureData texturedData = {
            .wallModel = cubeTexturedModel
    };

    Scene scene(texturedData, map, texturedShader);

    //Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    while (state.isRunning()) {
        unsigned int t_start = SDL_GetTicks();

        // reset movement
        state.movement = Movement::Default();

        while (SDL_PollEvent(&windowEvent)) {
            switch (windowEvent.type) {
                case SDL_QUIT:
                    state.quit = true;
                    break;
                case SDL_KEYUP:
                    handleKeyPress(state, windowEvent.key.keysym.sym);
                    break;
                case SDL_KEYDOWN:
                    handleKeyHold(state, windowEvent.key.keysym.sym);
            }

            SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        }


        switch (state.movement.look) {
            case Look::NONE:
                break;
            case Look::LEFT:
                state.angle -= LOOK_SPEED;
                break;
            case Look::RIGHT:
                state.angle += LOOK_SPEED;
                break;
        }

        glm::vec3 dir(-STRAFE_SPEED * cos(state.angle), STRAFE_SPEED * sin(state.angle), 0.0f);

        switch (state.movement.strafe) {
            case Strafe::NONE:
                break;
            case Strafe::FORWARD:
                state.camPosition += dir;
                break;
            case Strafe::BACKWARD:
                state.camPosition -= dir;
                break;
        }

        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(texturedShader); //Set the active shader (only one can be used at cubeTexturedModel time)

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

        unsigned int t_now = SDL_GetTicks();

        timePast = static_cast<float>(t_now) / 1000.f;

        glm::mat4 model = glm::mat4(1);
        GLint modelLoc = glGetUniformLocation(texturedShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::vec3 center = state.camPosition + dir;
        glm::vec3 up(0.0f, 0.0f, 1.0f);

        // set view matrix
        glm::mat4 view = glm::lookAt(state.camPosition, center, up);

        GLint uniView = glGetUniformLocation(texturedShader, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 proj = glm::perspective(FOV_Y, aspect, 1.0f, 10.0f);
        GLint uniProj = glGetUniformLocation(texturedShader, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vao);

        scene.Draw();
//        Utils::drawGeometry(texturedShader, cubeModel, model2, 1.0, 0.0, 0.0f);

        SDL_GL_SwapWindow(window); //Double buffering

        unsigned int t_end = SDL_GetTicks();
        char update_title[100];
        unsigned int time_per_frame = t_end - t_start;
        avg_render_time = .98f * avg_render_time + .02f * static_cast<float>(time_per_frame); //Weighted average for smoothing
        sprintf(update_title, "%s [Update: %3.0f ms]\n", window_title, static_cast<double>(avg_render_time));
        SDL_SetWindowTitle(window, update_title);
    }


    //Clean Up
    glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

