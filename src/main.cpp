#include <iostream>
#include <thread>

#include <argparse/argparse.hpp>

#include <fmt/core.h>
#include <fmt/color.h>

#include "../include/Solver.hpp"

constexpr int DEFAULT_SEED = 42;
constexpr int DEFAULT_ITERATIONS = 1000;

auto printResults(std::string_view kind, long time) -> void {
    fmt::print("[info][Time] :: {} time: ", kind);
    fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "{}", time);
    fmt::print("microseconds \n");
}

int main(int argc, char** argv) {

    argparse::ArgumentParser program("jacobi");
    program.add_description("Solve linear system equations using Jacobi method.");

    program.add_argument("size")
            .help("matrix and vector sizes to generate")
            .scan<'i', int>();

    program.add_argument("nw")
            .help("number of parallel workers to spawn")
            .default_value(std::thread::hardware_concurrency())
            .scan<'i', int>();

    program.add_argument("seed")
            .help("pseudo-random number generator's seed")
            .default_value(DEFAULT_SEED)
            .scan<'i', int>();


    program.add_argument("iterations")
            .help("how many Jacobi method iteration to perform")
            .default_value(DEFAULT_ITERATIONS)
            .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto seed = program.get<int>("seed");
    auto size = program.get<int>("size");
    auto nw = program.get<int>("nw");
    auto iterations = program.get<int>("iterations");

    auto A = spm::generateMatrix<double>(size, spm::MatrixType::DIAGONALLY_DOMINANT, seed);
    auto b = spm::generateVector<double>(size, seed);

    long timeSequential = 0;
    long timeParallel = 0;
    long timeFastFlow1 = 0;

    fmt::print("[info][Main] Solving linear system (Matrix Size = {} x {}, Vector Size = {}, Iterations = {}, Seed = {}, Workers = {})...\n", size, size, size, iterations, seed, nw);

    // We can discard results
    spm::solveJacobiSequential(A, b, iterations, &timeSequential);
    spm::solveJacobiParallel(A, b, iterations, nw, &timeParallel);
    spm::solveJacobiFastFlowPF(A, b, iterations, nw, &timeFastFlow1);

    printResults("Sequential", timeSequential);
    printResults("Native C++ Threads", timeParallel);
    printResults("FastFlow ParallelFor", timeFastFlow1);

    return 0;
}
