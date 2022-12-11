cmake_minimum_required(VERSION 3.20)
project(mdpc_gf4_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")

add_executable(mdpc_gf4_cpp main.cpp)
