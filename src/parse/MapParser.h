#pragma once


#include <string>
#include <repr/Map.h>
#include <fstream>

class MapParser {
public:
    Map parseMap(const std::string &name);
};

