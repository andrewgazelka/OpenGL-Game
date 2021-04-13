//
// Created by Andrew Gazelka on 4/13/21.
//

#include "MapParser.h"

/**
 *
 * @param c The character to parse
 * @return an element if valid else empty
 */
std::optional<Element> findElement(char c) {
    switch (c) {
        case '0':
            return Element::Empty();
        case 'S':
            return Element::Start();
        case 'G':
            return Element::Finish();
        case 'W':
            return Element::Wall();
        default:
            break;
    }

    if ('a' <= c && c <= 'e') return Element::Key(c - 'a');
    if ('A' <= c && c <= 'E') return Element::Key(c - 'A');

    return {};
}


Map MapParser::parseMap(const std::string &name) {
    std::ifstream file;
    file.open(name);
    size_t width, height;
    file >> width >> height;

    Map map = {
            .width = width,
            .height = height,
    };

    std::string line;

    size_t heightCount = 0;
    while (getline(file, line)) {
        heightCount += 1;
        size_t widthCount = 0;
        for (const auto &character : line) {
            const auto elem = findElement(character);
            if (!elem.has_value()) {
                fprintf(stderr, "Invalid character '%c' in file read\n", character);
                exit(1);
            }
            map.elements.push_back(elem.value());
            widthCount += 1;
        }
        if (widthCount != width) {
            fprintf(stderr, "Width of elements is %zu not the specified width %zu", widthCount, width);
            exit(1);
        }
    }
    if (heightCount != height) {
        fprintf(stderr, "Height of elements is %zu not the specified height %zu", heightCount, height);
        exit(1);
    }
    file.close(); // can probably be omitted
    return map;
}
