#pragma once

#include <string>

struct Model {
    unsigned long numLines;
    unsigned int numTriangles;
    float *data;
};

namespace Utils {
    Model loadModel(const std::string &name);
}


