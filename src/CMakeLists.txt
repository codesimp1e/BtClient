cmake_minimum_required(VERSION 3.5)

file(GLOB bt_src *.cc)
find_library(cryptopp cryptopp)
find_library(ssl ssl)
find_library(url boost_url)
find_library(crypto crypto)

set(CMAKE_CXX_STANDARD 14)
add_library(bt SHARED ${bt_src})

target_link_libraries(bt ${cryptopp} ${ssl} ${crypto} ${url})