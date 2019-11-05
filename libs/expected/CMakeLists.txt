cmake_minimum_required(VERSION 3.14)
project(tl-expected
  HOMEPAGE_URL https://tl.tartanllama.xyz
  DESCRIPTION "C++11/14/17 std::expected with functional-style extensions"
  VERSION 1.0.0
  LANGUAGES CXX)

add_library(expected INTERFACE)
target_include_directories(expected
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)