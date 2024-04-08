cmake_minimum_required(VERSION 3.27)
project(hungarian_algorithm_cpp)

set(CMAKE_CXX_STANDARD 14)

# Find Boost
find_package( Boost REQUIRED COMPONENTS program_options system filesystem)
include_directories( ${Boost_INCLUDE_DIRS} )

add_executable(hungarian_assign main.cpp
        hungarian.hpp hungarian.cpp)




