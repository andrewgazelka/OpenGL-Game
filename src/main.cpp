#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdio>

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

using namespace std;

bool saveOutput = false;
float timePast = 0;

const float FOV_Y = 3.14f / 4;
const float STRAFE_SPEED = 0.07;
const float LOOK_SPEED = 0.03;

// Shader sources
const GLchar *vertexSource = GLSL(
                                     in vec3 position;
                                     const vec3 inColor = vec3(0.f, 0.7f, 0.f);
                                     in vec3 inNormal;
                                     out vec3 Color;
                                     out vec3 normal;
                                     out vec3 pos;
                                     uniform mat4 model;
                                     uniform mat4 view;
                                     uniform mat4 proj;
                                     void main() {
                                         Color = inColor;
                                         gl_Position = proj * view * model * vec4(position, 1.0);
                                         pos = (model * vec4(position, 1.0)).xyz;
                                         vec4 norm4 = transpose(inverse(model)) * vec4(inNormal, 0.0);
                                         normal = normalize(norm4.xyz);
                                     }
                             );

const GLchar *fragmentSource = GLSL(
                                       in vec3 Color;
                                       in vec3 normal;
                                       in vec3 pos;
                                       out vec4 outColor;
                                       const vec3 lightDir = normalize(vec3(1, 1, 1));
                                       const float ambient = .3;
                                       void main() {
                                           vec3 diffuseC = Color * max(dot(lightDir, normal), 0.0);
                                           vec3 ambC = Color * ambient;
                                           vec3 reflectDir = reflect(lightDir, normal);
                                           vec3 viewDir = normalize(-pos);
                                           float spec = max(dot(reflectDir, viewDir), 0.0);
                                           if (dot(lightDir, normal) <= 0.0)spec = 0;
                                           vec3 specC = vec3(1.0, 1.0, 1.0) * pow(spec, 4);
                                           outColor = vec4(diffuseC + ambC + specC, 1.0);
                                       }
                               );

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

void Win2PPM(int width, int height);

int main(int argc, char *argv[]) {

    State state{
        .camPosition = glm::vec3(3.f, 0.f, 0.f)
    };

    Utils::SDLInit();

    //Create a window (offsetx, offsety, width, height, flags)
    SDL_Window *window = SDL_CreateWindow(window_title, 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    float aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

    //Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    Utils::loadGlad();

    Model loadModel = Utils::loadModel("models/teapot.txt");

    auto vertexShader = Utils::loadVertexShader(vertexSource);
    auto fragmentShader = Utils::loadFragmentShader(fragmentSource);

    //Join the vertex and fragment shaders together into one program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
    glLinkProgram(shaderProgram); //run the linker

    glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)


    //Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
    GLuint vao;
    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao); //Bind the above created VAO to the current context

    //Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo[1];
    glGenBuffers(1, vbo);  //Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)

    long bufferSize = static_cast<long>(loadModel.numLines * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, bufferSize, loadModel.data, GL_STATIC_DRAW); //upload vertices to vbo
    //GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
    //GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

    //Tell OpenGL how to set fragment shader input
    auto posAttrib = static_cast<unsigned int>(glGetAttribLocation(shaderProgram, "position"));

    int glStride = 8 * sizeof(float);

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, glStride, nullptr);
    glEnableVertexAttribArray(posAttrib);

    //GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);

    auto normAttrib = static_cast<unsigned int>(glGetAttribLocation(shaderProgram, "inNormal"));
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float))); // TODO: what???
    glEnableVertexAttribArray(normAttrib);

    glBindVertexArray(0); //Unbind the VAO

    //You need a second VAO, e.g., if some of the models are stored in a second format
    //Here is what that looks like--
    //GLuint vao2;
    //glGenVertexArrays(1, &vao2); //Create the VAO
    //glBindVertexArray(vao2); //Bind the above created VAO to the current context
    //  Creat VBOs ...
    //  Set-up attributes ...
    //glBindVertexArray(0); //Unbind the VAO


    glEnable(GL_DEPTH_TEST);

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

        unsigned int t_now = SDL_GetTicks();
        if (!saveOutput) timePast = static_cast<float>(t_now) / 1000.f;
        if (saveOutput) timePast += static_cast<float>(.07); //Fix framerate at 14 FPS

        glm::mat4 model = glm::mat4(1);
        GLint uniModel = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));


        glm::vec3 center = state.camPosition + dir;
        glm::vec3 up(0.0f, 0.0f, 1.0f);

        // set view matrix
        glm::mat4 view = glm::lookAt(state.camPosition, center, up);

        GLint uniView = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 proj = glm::perspective(FOV_Y, aspect, 1.0f, 10.0f);
        GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vao); // TODO: huh
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(loadModel.numTriangles)); //(Primitives, Which VBO, Number of vertices)
        if (saveOutput) Win2PPM(screen_width, screen_height);

        SDL_GL_SwapWindow(window); //Double buffering

        unsigned int t_end = SDL_GetTicks();
        char update_title[100];
        unsigned int time_per_frame = t_end - t_start;
        avg_render_time = .98f * avg_render_time + .02f * static_cast<float>(time_per_frame); //Weighted average for smoothing
        sprintf(update_title, "%s [Update: %3.0f ms]\n", window_title, static_cast<double>(avg_render_time));
        SDL_SetWindowTitle(window, update_title);
    }


    //Clean Up
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}


//Write out PPM image from screen
void Win2PPM(int width, int height) {
    char outdir[10] = "out/"; //Must be defined!
    int i, j;
    FILE *fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;

    /* Allocate our buffer for the image */
    image = (unsigned char *) malloc(static_cast<unsigned long>(3 * width * height) * sizeof(char));
    if (image == nullptr) {
        fprintf(stderr, "ERROR: Failed to allocate memory for image\n");
    }

    /* Open the file */
    sprintf(fname, "%simage_%04d.ppm", outdir, counter);
    if ((fptr = fopen(fname, "w")) == nullptr) {
        fprintf(stderr, "ERROR: Failed to open file to write image\n");
    }

    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

    /* Write the PPM file */
    fprintf(fptr, "P6\n%d %d\n255\n", width, height);
    for (j = height - 1; j >= 0; j--) {
        for (i = 0; i < width; i++) {
            fputc(image[3 * j * width + 3 * i + 0], fptr);
            fputc(image[3 * j * width + 3 * i + 1], fptr);
            fputc(image[3 * j * width + 3 * i + 2], fptr);
        }
    }

    free(image);
    fclose(fptr);
    counter++;
}
