cmake_minimum_required(VERSION 3.25)
project(vector)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(data)

add_executable(vector
        data/four/code.cpp
        data/one/code.cpp
        data/three/code.cpp
        data/two/code.cpp
        data/class-bint.hpp
        data/class-integer.hpp
        data/class-matrix.hpp
        exceptions.hpp
        utility.hpp
        vector.hpp)
