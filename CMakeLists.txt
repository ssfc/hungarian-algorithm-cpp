cmake_minimum_required(VERSION 3.27)
project(hungarian_algorithm_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(hungarian_assign main.cpp
        hungarian.hpp hungarian.cpp)




