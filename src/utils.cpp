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
        for (int i = 0; i < numLines; i++) {
            file >> modelData[i];
        }
        unsigned int numTris = numLines / 8;

        return {
                .numLines = numLines,
                .numTriangles = numTris,
                .data = modelData
        };
    }

}