#include <vector>
#include <barrier>
#include <thread>
#include <string_view>

#include "../../include/Solver.hpp"

spm::VectorD spm::solveJacobiParallel(const spm::MatrixD& A, const spm::VectorD& b, unsigned iterations, unsigned parallelDegree, long* time) {

    using namespace std::string_view_literals;

    auto matrixSize = A.size();
    auto oldSolution = spm::VectorD(matrixSize, 1);
    auto newSolution = spm::VectorD(matrixSize, 0);

    auto threads = std::vector<std::thread>{};
    auto syncPoint = std::barrier(parallelDegree, [&]() {
        // Update the old solution with the new one for the next iteration.
        oldSolution = newSolution;
        // Decrease the number of iterations by reference.
        iterations--;
    });

    auto task = [&](std::size_t from, std::size_t to) -> void {

        auto matrixSize = A.size();

        while (iterations > 0) {
            for (std::size_t i = from; i <= to; i++) {
                double sigma = 0.0;
                for (std::size_t j = 0; j < matrixSize; j++) {
                    if (i != j) {
                        sigma += A(i, j) * oldSolution[j];
                    }
                }
                newSolution[i] = ((b[i] - sigma) / A(i, i));
            }

            syncPoint.arrive_and_wait();
        }
    };

    {
#ifdef TRACE_PERFORMANCE
        spm::Timer performanceTimer("Native threads Jacobi method"sv, time);
#endif
        // Allocate threads and assign to each one of them
        // a chunk to compute.
        std::size_t offset = 0;

        auto delta = (matrixSize / parallelDegree);
        for (std::size_t _ = 0; _ < parallelDegree; _++, offset += delta) {
            threads.emplace_back(task, offset, (_ == parallelDegree - 1) ? (matrixSize - 1) : offset + delta - 1);
        }

        // Wait for thread's termination.
        for (auto& thread: threads) thread.join();
    }

    return newSolution;
}
