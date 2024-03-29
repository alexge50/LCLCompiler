cmake_minimum_required(VERSION 3.9)
project(lcl)

set(CMAKE_CXX_STANDARD 17)
option(EXPECTED_BUILD_TESTS "..." OFF)
option(UTF8_TESTS "..." OFF)

add_subdirectory(libs/Catch2)
add_subdirectory(libs/expected)
add_subdirectory(libs/fmt)
add_subdirectory(libs/GSL)
add_subdirectory(libs/magic_enum)
add_subdirectory(libs/range-v3)
add_subdirectory(libs/utf8proc)
add_subdirectory(libs/utfcpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS sources/*)
add_executable(lcl ${SOURCES})
target_link_libraries(lcl PRIVATE Catch2 expected fmt GSL magic_enum::magic_enum range-v3 utf8proc utfcpp)
target_include_directories(lcl PRIVATE sources/)

add_executable(lcl_test_tokenizer tests/test_tokenizer.cpp)
target_link_libraries(lcl PRIVATE Catch2 expected fmt GSL magic_enum::magic_enum range-v3 utf8proc utfcpp)
target_include_directories(lcl PRIVATE sources/)
