#include <fstream>
#include <boost/format.hpp>
#include "utils.h"

namespace Utils {

    Model loadModel(const std::string &name) {
        std::ifstream file;
        file.open(name);

        if (!file.is_open()) {
            const auto msg = boost::format{"File %1% is not open for reading"} % name;
            throw std::invalid_argument(msg.str());
        }

        unsigned long numLines = 0;
        file >> numLines;
        auto *modelData = new float[numLines];
        for (unsigned int i = 0; i < numLines; i++) {
            file >> modelData[i];
        }
        auto numTris = static_cast<unsigned int>(numLines / 8);

        return {
                .numLines = numLines,
                .numTriangles = numTris,
                .data = modelData
        };
    }

    unsigned int loadVertexShader(const GLchar *vertexSource) {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);

        //Let's double check the shader compiled
        GLint status;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if (!status) {
            char buffer[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, buffer);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Compilation Error",
                                     "Failed to Compile: Check Consol Output.",
                                     nullptr);

            printf("Vertex Shader Compile Failed. Info:\n\n%s\n", buffer);
        }

        return vertexShader;
    }

    unsigned int loadFragmentShader(const GLchar *fragmentSource) {
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);

        GLint status;

        //Double check the shader compiled
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        if (!status) {
            char buffer[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, buffer);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Compilation Error",
                                     "Failed to Compile: Check Consol Output.",
                                     nullptr);
            printf("Fragment Shader Compile Failed. Info:\n\n%s\n", buffer);
        }
        return fragmentShader;
    }

    void loadGlad() {
        if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            printf("\nOpenGL loaded\n");
            printf("Vendor:   %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version:  %s\n\n", glGetString(GL_VERSION));
        } else {
            throw std::logic_error("ERROR: Failed to initialize OpenGL context.");
        }
    }

    void SDLInit() {
        SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

        //Ask SDL to get a recent version of OpenGL (3.2 or greater)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    }

}