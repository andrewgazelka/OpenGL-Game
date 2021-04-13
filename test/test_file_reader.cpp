#include <MapParser.h>
#include "gtest/gtest.h"

const auto FILE_NAME = "/Users/andrewgazelka/Projects/School/5607-cg/proj4/test/test.txt";

namespace {

    TEST(MapParser, WidthHeightRight) {
        Map map = MapParser::parseMap(FILE_NAME);
        EXPECT_EQ(map.height, 5);
        EXPECT_EQ(map.width, 5);
    }
}

