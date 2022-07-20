cmake_minimum_required(VERSION 3.23)
project(Jacobi_SPM)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(TRACE_PERFORMANCE)

add_executable(Jacobi_SPM
        src/main.cpp
        include/Timer.hpp
        include/Common.hpp
        include/Solver.hpp
        src/SequentialSolver.cpp
        src/NativeThreadSolver.cpp
        src/FastFlowSolver.cpp)

include_directories(lib/fastflow)
link_directories(lib/fastflow)