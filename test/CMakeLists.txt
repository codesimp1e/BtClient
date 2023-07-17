cmake_minimum_required(VERSION 3.5)

file(GLOB test_src *.cc)

include_directories(../src)
add_executable(test ${test_src})
target_link_libraries(test bt)
