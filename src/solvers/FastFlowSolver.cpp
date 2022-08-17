#include "../../include/Solver.hpp"

#include <string_view>

#include <ff/parallel_for.hpp>

spm::VectorD spm::solveJacobiFastFlowPF(const spm::MatrixD &A, const spm::VectorD &b, unsigned int iterations,
                                        unsigned int parallelDegree, long *time) {

   using namespace std::string_view_literals;

    auto matrixSize = static_cast<long>(A.size());
    auto oldSolution = spm::VectorD(matrixSize, 1);
    auto newSolution = spm::VectorD(matrixSize, 0);

    ff::ParallelFor parallelFor(parallelDegree);

    {
#ifdef TRACE_PERFORMANCE
        spm::Timer performanceTimer("Parallel FastFlow Jacobi method"sv, time);
#endif
        while (iterations > 0) {

            parallelFor.parallel_for(0, matrixSize, 1, [matrixSize, &A, &b, &oldSolution, &newSolution](const long i) {
                double sigma = 0.0;
                for (std::size_t j = 0; j < matrixSize; j++) {
                    if (i != j) {
                        sigma += A(i, j) * oldSolution[j];
                    }
                }
                newSolution[i] = ((b[i] - sigma) / A(i, i));
            }, parallelDegree);

            oldSolution = newSolution;
            iterations--;
        }
    }

    return newSolution;
}
