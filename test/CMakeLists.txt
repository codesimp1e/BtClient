cmake_minimum_required(VERSION 3.5)

include_directories(../src)
add_executable(test test.cc)
target_link_libraries(test bt)

add_executable(test_begin test_begin.cc)