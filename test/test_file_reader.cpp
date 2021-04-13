#include <MapParser.h>
#include "gtest/gtest.h"

const auto FILE_NAME = "/Users/andrewgazelka/Projects/School/5607-cg/proj4/test/test.txt";

namespace {

    TEST(MapParser, WidthHeightRight) {
        Map map = MapParser::parseMap(FILE_NAME);
        EXPECT_EQ(map.height, 5);
        EXPECT_EQ(map.width, 5);
        EXPECT_EQ(map.GetElement(0,0), Element::Empty());
        EXPECT_EQ(map.GetElement(4,0), Element::Finish());
        EXPECT_EQ(map.GetElement(1,1), Element::Wall());
        EXPECT_EQ(map.GetElement(4,4), Element::Key(0));
        EXPECT_EQ(map.GetElement(2,2), Element::Door(0));
    }
}

