cmake_minimum_required(VERSION 3.23)
project(Jacobi_SPM)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(TRACE_PERFORMANCE)

FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_Declare(
        fastflow
        GIT_REPOSITORY https://github.com/fastflow/fastflow.git
)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)

FetchContent_MakeAvailable(argparse fastflow fmt)

# FastFlow is header-only
include_directories(${fastflow_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        src/main.cpp
        include/Timer.hpp
        include/Common.hpp
        include/Solver.hpp
        src/solvers/SequentialSolver.cpp
        src/solvers/NativeThreadSolver.cpp
        src/solvers/FastFlowSolver.cpp)

target_link_libraries(${PROJECT_NAME} argparse fmt)

