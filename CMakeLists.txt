cmake_minimum_required(VERSION 3.23)
project(Jacobi_SPM)

set(CMAKE_CXX_STANDARD 20)

add_executable(Jacobi_SPM src/main.cpp include/Timer.hpp include/Common.hpp)
