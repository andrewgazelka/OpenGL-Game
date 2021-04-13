message(STATUS "Loading test")

find_package(GTest REQUIRED)

enable_testing()
add_executable(basic_test test_file_reader.cpp)
target_link_libraries(basic_test PUBLIC GTest::gtest GTest::gtest_main proj4-lib)
