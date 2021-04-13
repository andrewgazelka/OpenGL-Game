cmake_minimum_required(VERSION 3.17)
project(proj4)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(src)
include_directories(src/parse)

add_subdirectory(lib/glad)
add_subdirectory(lib/glm)


add_executable(proj4 src/parse/MapParser.cpp src/parse/MapParser.h src/repr/Map.cpp src/repr/Map.h src/main.cpp)
target_link_libraries(proj4 glm glad ${SDL2_LIBRARIES} Boost::filesystem)
