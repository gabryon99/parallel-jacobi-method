#ifndef JACOBI_SPM_SOLVER_HPP
#define JACOBI_SPM_SOLVER_HPP

#include "Common.hpp"

namespace spm {

    VectorD solveJacobiSequential(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, long* time = nullptr);

    VectorD solveJacobiParallel(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, unsigned parallelDegree = 2, long* time = nullptr);

    VectorD solveJacobiFastFlowPF(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, unsigned parallelDegree = 2, long* time = nullptr);
}

#endif //JACOBI_SPM_SOLVER_HPP
