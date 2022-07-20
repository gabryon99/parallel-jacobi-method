#ifndef JACOBI_SPM_COMMON_HPP
#define JACOBI_SPM_COMMON_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

#include "Timer.hpp"

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

}

#endif //JACOBI_SPM_COMMON_HPP
