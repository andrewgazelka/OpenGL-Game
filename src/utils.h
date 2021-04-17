#pragma once

#include <string>
#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>


struct Model {
    unsigned long numLines;
    unsigned int numTriangles;
    float *data;
};

namespace Utils {
    Model loadModel(const std::string &name);
    unsigned int loadVertexShader(const GLchar * vertexSource);
    unsigned int loadFragmentShader(const GLchar *fragmentSource);
    void loadGlad();
    void SDLInit();
}


