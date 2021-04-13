#pragma once

#include <cstdlib>
#include <vector>

enum class Tag {
    START,
    FINISH,
    DOOR,
    KEY,
    WALL,
    EMPTY
};

struct Door {
    size_t id;
};

struct Key {
    size_t id;
};

struct Empty {
    static Empty Get(){
        return {};
    }
};


struct Element {
    Tag tag;
    union Value {
        Empty empty;
        Door door;
        Key key;
    } value;

    static Element Start();
    static Element Finish();
    static Element Empty();
    static Element Wall();
    static Element Door(size_t id);
    static Element Key(size_t id);
};

struct Map {
    size_t width, height;
    std::vector<Element> elements;

    [[nodiscard]] Element GetElement(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        return elements[y * width + x];
    }
};

