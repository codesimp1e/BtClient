cmake_minimum_required(VERSION 3.5)

file(GLOB bt_src *.cc)
find_library(cryptopp cryptopp)

add_library(bt SHARED ${bt_src})

target_link_libraries(bt ${cryptopp})