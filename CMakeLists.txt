cmake_minimum_required(VERSION 3.27)
project(hungarian_cpp_from_python)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR c:/MinGW_distro/include/)
set(Boost_LIBRARY_DIR c:/MinGW_distro/lib/)

# Find Boost
find_package( Boost REQUIRED COMPONENTS program_options)
include_directories( ${Boost_INCLUDE_DIRS} )

add_executable(hungarian_assign main.cpp
        hungarian.hpp hungarian.cpp)

target_link_libraries(hungarian_assign ${Boost_LIBRARIES})


