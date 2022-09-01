#include <iostream>
#include <thread>

#include <argparse/argparse.hpp>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fstream>

#include "../include/Solver.hpp"

auto printResults(std::string_view kind, long time) -> void {
    fmt::print("[info][Main] :: {} time: ", kind);
    fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "{}", time);
    fmt::print(" microseconds \n");
}

int main(int argc, char** argv) {

    argparse::ArgumentParser program("Jacobi_SPM");
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
            .default_value(spm::Constants::DEFAULT_SEED)
            .scan<'i', int>();

    program.add_argument("iterations")
            .help("how many Jacobi method iterations to perform")
            .default_value(spm::Constants::DEFAULT_ITERATIONS)
            .scan<'i', int>();

    program.add_argument("-o", "--output")
            .required()
            .help("specify the output CSV file containing execution times.");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        // We didn't succeed in parsing the arguments, exit the program.
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto seed = program.get<int>("seed");
    auto size = program.get<int>("size");
    auto nw = program.get<int>("nw");
    auto iterations = program.get<int>("iterations");
    auto outputFilename = program.get<std::string>("--output");

    auto A = spm::generateMatrix<double>(size, spm::MatrixType::DIAGONALLY_DOMINANT, seed);
    auto b = spm::generateVector<double>(size, seed);

    long timeSequential = 0;
    long timeParallel = 0;
    long timeFastFlow1 = 0;

    fmt::print("[info][Main] Solving linear system (Matrix Size = {} x {}, Vector Size = {}, Iterations = {}, Seed = {}, Workers = {})...\n", size, size, size, iterations, seed, nw);

    // We can discard the results
    spm::solveJacobiSequential(A, b, iterations, &timeSequential);
    spm::solveJacobiParallel(A, b, iterations, nw, &timeParallel);
    spm::solveJacobiFastFlow(A, b, iterations, nw, &timeFastFlow1);

    double speedupParallel = static_cast<double>(timeSequential) / static_cast<double>(timeParallel);
    double speedupFastFlow = static_cast<double>(timeSequential) / static_cast<double>(timeFastFlow1);

    double efficiencyParallel = speedupParallel / nw;
    double efficiencyFastFlow = speedupFastFlow / nw;

    // Write collected measures into a csv file
    std::ofstream csvFile;
    csvFile.open(outputFilename, std::ios::app | std::ios::out);
    // In order:
    // Size ,Workers, Parallel Time, FastFlow Time, Speedup Parallel, Speedup FastFlow, Efficiency Parallel, Efficiency Speedup
    csvFile << fmt::format("{},{},{},{},{},{},{},{}\n",
                           size, nw,
                           timeParallel, timeFastFlow1,
                           speedupParallel, speedupFastFlow,
                           efficiencyParallel, efficiencyFastFlow);
    csvFile.close();

    return 0;
}
