#ifndef JACOBI_SPM_SOLVER_HPP
#define JACOBI_SPM_SOLVER_HPP

#include "Common.hpp"

namespace spm {

    /***
     * Solve the equation Ax = b sequentially, using the standard Jacobi's method.
     */
    VectorD solveJacobiSequential(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, long* time = nullptr);

    /***
     * Solve the equation Ax = b in parallel, using the standard thread C++ library.
     */
    VectorD solveJacobiParallel(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, unsigned parallelDegree = 2, long* time = nullptr);

    /***
     * Solve the equation Ax = b in parallel, using the FastFlow framework.
     */
    VectorD solveJacobiFastFlow(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, unsigned parallelDegree = 2, long* time = nullptr);
}

#endif //JACOBI_SPM_SOLVER_HPP
