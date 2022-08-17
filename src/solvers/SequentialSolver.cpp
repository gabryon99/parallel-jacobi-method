#include "../../include/Solver.hpp"

spm::VectorD spm::solveJacobiSequential(const spm::MatrixD& A, const spm::VectorD& b, unsigned iterations, long* time) {

    using namespace std::string_view_literals;

    auto matrixSize = A.size();
    auto oldSolution = VectorD(matrixSize, 1);
    auto newSolution = VectorD(matrixSize, 0);

    {
#ifdef TRACE_PERFORMANCE
        spm::Timer performanceTimer("Sequential Jacobi method"sv, time);
#endif
        for (/* Empty */; iterations > 0; iterations--) {

            for (std::size_t i = 0; i < matrixSize; i++) {

                double sigma = 0.0;

                for (std::size_t j = 0; j < matrixSize; j++) {
                    if (i != j) {
                        sigma += A(i, j) * oldSolution[j];
                    }
                }

                newSolution[i] = ((b[i] - sigma) / A(i, i));
            }

            oldSolution = newSolution;
        }
    }

    return newSolution;
}
