#include <vector>
#include <barrier>
#include <thread>
#include <string_view>

#include "../include/Solver.hpp"

spm::VectorD spm::solveJacobiParallel(const spm::MatrixD& A, const spm::VectorD& b, unsigned iterations, unsigned parallelDegree, long* time) {

    using namespace std::string_view_literals;

    auto matrixSize = A.size();
    auto oldSolution = spm::VectorD(matrixSize, 1);
    auto newSolution = spm::VectorD(matrixSize, 0);

    auto threads = std::vector<std::thread>{};
    auto syncPoint = std::barrier(parallelDegree, [&iterations]() {
        iterations--;
    });

    auto task = [&iterations, &A, &b, &syncPoint, &oldSolution, &newSolution](std::size_t from, std::size_t to) -> void {

        auto matrixSize = A.size();

        while (iterations > 0) {
            for (std::size_t i = from; i <= to; i++) {
                double sigma = 0.0;
                for (std::size_t j = 0; j < matrixSize; j++) {
                    if (i != j) {
                        sigma += A[i][j] * oldSolution[j];
                    }
                }
                newSolution[i] = (1.0 / A[i][i]) * (b[i] - sigma);
            }

            syncPoint.arrive_and_wait();
            for (std::size_t i = from; i <= to; i++) oldSolution[i] = newSolution[i];
        }
    };

    // Allocate threads
    std::size_t offset = 0;
    auto delta = (matrixSize / parallelDegree);
    for (std::size_t _ = 0; _ < parallelDegree; _++, offset += delta) {
        threads.emplace_back(task, offset, (_ == parallelDegree - 1) ? (matrixSize - 1) : offset + delta - 1);
    }

    {
#ifdef TRACE_PERFORMANCE
        spm::Timer performanceTimer("Native threads Jacobi method"sv, time);
#endif
        for (auto& thread: threads) thread.join();
    }

    return newSolution;
}
