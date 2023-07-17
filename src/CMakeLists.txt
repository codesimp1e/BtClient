cmake_minimum_required(VERSION 3.5)

file(GLOB bt_src *.cc)

add_library(bt SHARED ${bt_src})