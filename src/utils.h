#pragma once

#include <string>
#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>
#include <vec3.hpp>
#include <type_ptr.hpp>


struct Model {

    unsigned long numLines = 0;
    unsigned int numVertices = 0;
    unsigned int start = 0;
    std::unique_ptr<float[]> data = nullptr;

    static Model combine(std::initializer_list<Model*> models) {

        // first pass... figure out the number of lines
        unsigned long numLines = 0;
        for (auto &model: models) {
            numLines += model->numLines;
        }

        auto data = new float[numLines];

        // second pass create data array
        int i = 0;
        int iStart = 0;
        for (auto &model: models) {
            for (int j = 0; j < model->numLines; ++j) {
                data[i] = model->data[j];
                i++;
            }
            model->data = std::unique_ptr<float[]>(&data[iStart]);
            model->start = iStart;
            iStart = i;
        }


        return {
                .numLines = numLines,
                .numVertices = static_cast<unsigned int>(numLines / 8),
                .data = std::unique_ptr<float[]>(data)
        };

    }
};

namespace Utils {
    Model loadModel(const std::string &name);

    void loadGlad();

    void SDLInit();

    GLuint InitShader(const char *vShaderFileName, const char *fShaderFileName);

// Create a NULL-terminated string by reading the provided file
    char *readShaderSource(const char *shaderFile);

    void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, float colR, float colG, float colB);

    unsigned int loadBMP(const std::string &filePath);

}


