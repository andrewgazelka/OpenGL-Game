#include <MapParser.h>
#include "gtest/gtest.h"

const auto FILE_NAME = "/Users/andrewgazelka/Projects/School/5607-cg/proj4/test/test.txt";
const auto INVALID_FILE = "/Users/andrewgazelka/Projects/School/5607-cg/proj4/test/invalid.txt";

namespace {

    TEST(MapParser, MapGeneratedProperly) {
        Map map = MapParser::parseMap(FILE_NAME);
        EXPECT_EQ(map.height, 5);
        EXPECT_EQ(map.width, 5);
        EXPECT_EQ(map.GetElement(0, 0), Element::Empty());
        EXPECT_EQ(map.GetElement(4, 0), Element::Finish());
        EXPECT_EQ(map.GetElement(1, 1), Element::Wall());
        EXPECT_EQ(map.GetElement(4, 4), Element::Key(0));
        EXPECT_EQ(map.GetElement(2, 2), Element::Door(0));
    }

    TEST(MapParser, InvalidDetected) {
        EXPECT_THROW({ Map map = MapParser::parseMap(INVALID_FILE); },
                     std::invalid_argument);

    };
}

