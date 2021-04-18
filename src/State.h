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
    float velocityY = 0.0;
    static Movement Default(){
        Movement movement;
        return movement;
    }
};



struct State {
    bool quit = false;
    bool fullscreen = false;
    glm::vec3 camPosition;
    float angle;
    Movement movement;


    [[nodiscard]] inline bool isRunning() const{
        return !quit;
    }

    inline bool onGround(){
        return camPosition[2] == 0.0f;
    }
};

