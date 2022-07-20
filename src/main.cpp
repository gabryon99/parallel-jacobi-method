#include <iostream>

#include "../include/Solver.hpp"

int main() {

    constexpr std::size_t size = 128;

    auto A = spm::generateMatrix<double>(size, spm::MatrixType::DIAGONALLY_DOMINANT);
    auto b = spm::generateVector<double>(size);

    long timeSequential = 0;
    long timeParallel = 0;
    long timeFastFlow1 = 0;

    auto x1 = spm::solveJacobiSequential(A, b, 1000, &timeSequential);
    auto x2 = spm::solveJacobiParallel(A, b, 1000, 8, &timeParallel);
    auto x3 = spm::solveJacobiFastFlow(A, b, 1000, 8, &timeFastFlow1);

    spm::printVector(x1);
    spm::printVector(x2);
    spm::printVector(x3);

    std::cout << "Sequential: " << timeSequential << "\n";
    std::cout << "Parallel: " << timeParallel << "\n";
    std::cout << "ParallelFor FastFlow: " << timeFastFlow1 << "\n";

    std::cout << "Speedup Native: " << (static_cast<double>(timeSequential) / timeParallel) << "\n";
    std::cout << "Speedup ParallelFor: " << (static_cast<double>(timeSequential) / timeFastFlow1) << "\n";

    return 0;
}
