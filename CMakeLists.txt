cmake_minimum_required(VERSION 3.22)
project(Jacobi_SPM)

include(FetchContent)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O1")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_compile_definitions(TRACE_PERFORMANCE USE_1D_REPRESENTATION)

function(NEW_TEST_SUITE MATRIX_SIZE)
    foreach(test RANGE 1 3 1)
        # Useful to measure Scalability
        message(STATUS "Creating new test suite (Matrix Size: ${MATRIX_SIZE}, Workers: 1)")
        add_test(Matrix_${MATRIX_SIZE}_1 ${PROJECT_NAME} -o output_${MATRIX_SIZE}_${test}.csv ${MATRIX_SIZE} 1 42 1000)
        foreach (nw RANGE 2 32 2)
            message(STATUS "Creating new test suite (Matrix Size: ${MATRIX_SIZE}, Workers: ${nw})")
            add_test(Matrix_${MATRIX_SIZE}_${nw} ${PROJECT_NAME} -o output_${MATRIX_SIZE}_${test}.csv ${MATRIX_SIZE} ${nw} 42 1000)
        endforeach ()
    endforeach()
endfunction()

# Fetch argparse from GitHub to parse arguments from command line
# in a modern C++ way.
message(STATUS "Fetching 'argparse' from GitHub...")
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
message(STATUS "Fetching 'FastFlow' from GitHub...")
FetchContent_Declare(
        fastflow
        GIT_REPOSITORY https://github.com/fastflow/fastflow.git
)
message(STATUS "Fetching 'fmt' from GitHub...")
# Fetch fmt library from GitHub to format strings easily.
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

# Define tests
new_test_suite(256)
new_test_suite(512)
new_test_suite(1024)
new_test_suite(2048)
new_test_suite(4096)
new_test_suite(8192)
new_test_suite(16384)
