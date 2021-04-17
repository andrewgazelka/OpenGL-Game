#pragma once

#include <vec3.hpp>

enum class Look {
    NONE,
    LEFT,
    RIGHT
};

enum class Strafe {
    NONE,
    FORWARD,
    BACKWARD
};


struct Movement {
    Look look = Look::NONE;
    Strafe strafe = Strafe::NONE;
    static Movement Default(){
        Movement movement;
        return movement;
    }
};



struct State {
    bool quit = false;
    bool fullscreen = false;
    glm::vec3 camPosition;
    Movement movement;


    [[nodiscard]] inline bool isRunning() const{
        return !quit;
    }
};

