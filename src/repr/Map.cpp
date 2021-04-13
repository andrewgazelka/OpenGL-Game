//
// Created by Andrew Gazelka on 4/13/21.
//


#include "Map.h"

Element Element::Door(size_t id) {

    struct Door d{
        .id = id
    };

    return Element {
        .tag = Tag::DOOR,
        .value = {
                .door = d
        }
    };
}

Element Element::Wall() {
    return {
        .tag = Tag::WALL
    };
}

Element Element::Empty() {
    return {
        .tag = Tag::EMPTY
    };
}

Element Element::Key(size_t id) {
    return {
        .tag = Tag::KEY,
        .value = {
                .key = {
                        .id = id
                }
        }
    };
}

Element Element::Start() {
    return {.tag = Tag::START};
}

Element Element::Finish() {
    return {.tag = Tag::FINISH};
}

