#include <iostream>
#include <iomanip>
#include <vector>
#include <string_view>
#include <typeinfo>
#include <random>
#include <barrier>
#include <thread>

#include "../include/Timer.hpp"

#define TRACE_PERFORMANCE

namespace spm {

    enum class MatrixType {
        NORMAL,
        DIAGONAL,
        DIAGONALLY_DOMINANT,
    };

    constexpr int MIN = -128;
    constexpr int MAX = 128;

    template <typename T>
    concept Number = std::is_integral<T>::value || std::is_floating_point<T>::value;

    template <Number Num>
    using Vector = std::vector<Num>;

    template <std::floating_point Num>
    using Matrix = std::vector<std::vector<Num>>;

    using VectorD = Vector<double>;
    using MatrixD = Matrix<double>;

    /***
     * Prints out a vector on out channel.
     * @tparam T
     * @param v
     * @param out
     */
    template <typename T>
    auto printVector(const Vector<T> &v, std::ostream& out = std::cout) -> void {
        out << "[ ";
        for (const auto num: v) {
            if constexpr (std::is_floating_point<T>::value) {
                out << std::setw(4) << std::left << std::setprecision(4) << std::fixed << num << " ";
            }
            else {
                out << num << "\t";
            }
        }
        out << " ]\n";
    }

    /***
     * Prints out a matrix on out channel.
     * @tparam T
     * @param m
     * @param out
     */
    template <typename T>
    auto printMatrix(const Matrix<T> &m, std::ostream& out = std::cout) -> void {
        for (const auto& row: m) {
            printVector(row, out);
        }
    }

    template <Number T>
    auto generateMatrix(std::size_t size, MatrixType matrixType = MatrixType::NORMAL, unsigned seed = 42) -> Matrix<T> {

        std::random_device rd;
        std::default_random_engine eng(rd());
        eng.seed(seed);

        Matrix<T> generatedMatrix{};
        generatedMatrix.resize(size);
        for (auto& row : generatedMatrix) {
            row.resize(size);
        }

        auto genMatrix = [&generatedMatrix, &eng, size, &matrixType](auto distr) -> void {
            switch (matrixType) {
                case MatrixType::NORMAL: {
                    for (std::size_t i = 0; i < size; i++) {
                        for (std::size_t j = 0; j < size; j++) {
                            generatedMatrix[i][j] = distr(eng);
                        }
                    }
                    break;
                }
                case MatrixType::DIAGONAL: {

                    for (std::size_t i = 0; i < size; i++) {
                        for (std::size_t j = 0; j < i + 1; j++) {
                            generatedMatrix[i][j] = distr(eng);
                        }
                    }
                    break;
                }
                case MatrixType::DIAGONALLY_DOMINANT: {
                    for (std::size_t i = 0; i < size; i++) {

                        auto sum{ 0.0 };

                        for (std::size_t j = 0; j < size; j++) {
                            if (j != i) {
                                auto cell{ distr(eng) };
                                generatedMatrix[i][j] = cell;
                                sum = sum + cell;
                            }
                        }

                        generatedMatrix[i][i] = sum + distr(eng);
                    }
                    break;
                }
            }
        };

        if constexpr (std::is_same<T, double>()) {
            std::uniform_real_distribution<double> distr(MAX - MIN, MAX);
            genMatrix(distr);
        }
        else {
            std::uniform_int_distribution<T> distr(MIN, MAX);
            genMatrix(distr);
        }

        return generatedMatrix;
    }

    template <Number T>
    auto generateVector(std::size_t size, unsigned seed = 42) -> Vector<T> {

        std::random_device rd;
        std::default_random_engine eng(rd());
        eng.seed(seed);

        Vector<T> generatedVector{};
        generatedVector.resize(size);

        auto genVector = [&generatedVector, &eng, size](auto distribution) -> void {
            for (std::size_t i = 0; i < size; i++) {
                generatedVector[i] = distribution(eng);
            }
        };

        if constexpr (std::is_same<T, double>()) {
            std::uniform_real_distribution<double> distribution(MAX - MIN, MAX);
            genVector(distribution);
        }
        else {
            std::uniform_int_distribution<T> distribution(MAX - MIN, MAX);
            genVector(distribution);
        }

        return generatedVector;
    }

    /***
     * Solve the equation Ax = b using Jacobi method. The function assumes
     * that 'A' is squared and the rows(A) = rows(b).
     * @param A
     * @param b
     * @param iterations
     * @return
     */
    auto solveJacobiSequential(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, long* time = nullptr) -> VectorD {

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
                            sigma += A[i][j] * oldSolution[j];
                        }
                    }

                    oldSolution[i] = (newSolution[i] = (1.0 / A[i][i]) * (b[i] - sigma));
                }
            }
        }

        return newSolution;
    }


    auto solveJacobiParallel(const MatrixD& A, const VectorD& b, unsigned iterations = 1000, unsigned parallelDegree = 2, long* time = nullptr) -> VectorD {

        using namespace std::string_view_literals;

        auto matrixSize = A.size();
        auto oldSolution = VectorD(matrixSize, 1);
        auto newSolution = VectorD(matrixSize, 0);

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

}

int main() {

    constexpr std::size_t size = 4096;

    auto A = spm::generateMatrix<double>(size, spm::MatrixType::DIAGONALLY_DOMINANT);
    auto b = spm::generateVector<double>(size);

    long timeSequential = 0;
    long timeParallel = 0;

    auto x1 = spm::solveJacobiSequential(A, b, 1000, &timeSequential);
    auto x2 = spm::solveJacobiParallel(A, b, 1000, 8, &timeParallel);
    // spm::printVector(x1);
    // spm::printVector(x2);

    std::cout << "Sequential: " << timeSequential << "\n";
    std::cout << "Parallel: " << timeParallel << "\n";

    std::cout << "Speedup: " << (static_cast<double>(timeSequential) / timeParallel) << "\n";

    return 0;
}
