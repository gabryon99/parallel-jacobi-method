#ifndef JACOBI_SPM_COMMON_HPP
#define JACOBI_SPM_COMMON_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

#include "Timer.hpp"

namespace spm {

    namespace Constants {
        constexpr int MIN = -128;
        constexpr int MAX = 128;
        constexpr int DEFAULT_SEED = 42;
        constexpr int DEFAULT_ITERATIONS = 1000;
    }

    enum class MatrixType {
        NORMAL,
        DIAGONAL,
        DIAGONALLY_DOMINANT,
    };

    template <typename T>
    concept Number = std::is_integral<T>::value || std::is_floating_point<T>::value;

    template <Number Num>
    using Vector = std::vector<Num>;

    template <typename T>
    class Matrix_1D {
    public:

        using data_type         = std::vector<T>;
        using value_type        = typename data_type::value_type;
        using size_type         = typename data_type::size_type;

        using reference         = typename data_type::reference;
        using const_reference   = typename data_type::const_reference;

        Matrix_1D(size_type size) : m_size{size}, m_data(size * size) {}
        Matrix_1D(size_type size, T defaultValue) : m_size{size}, m_data(size * size, defaultValue) {}

        reference operator()(const size_type i, const size_type j) {
            return m_data[j + i * m_size];
        }

        const_reference operator()(const size_type i, const size_type j) const {
            return m_data[j + i * m_size];
        }

        size_type size() const { return m_size; }

    private:
        data_type m_data{};
        size_type m_size{};
    };

    template <typename T>
    class Matrix_2D {
    public:
        using linear_type       = std::vector<T>;
        using data_type         = std::vector<std::vector<T>>;
        using value_type        = typename linear_type::value_type;
        using size_type         = typename data_type::size_type;

        using reference         = typename linear_type::reference;
        using const_reference   = typename linear_type::const_reference;

        Matrix_2D(size_type size) : m_size{size} {
            m_data.resize(size);
            for (auto& row: m_data) {
                row.resize(size);
            }
        }

        Matrix_2D(size_type size, T defaultValue) : m_size{size} {
            m_data.resize(size);
            for (auto& row: m_data) {
                row.resize(size);
                std::fill(row.begin(), row.end(), defaultValue);
            }
        }

        reference operator()(const size_type i, const size_type j) {
            return m_data[i][j];
        }

        const_reference operator()(const size_type i, const size_type j) const {
            return m_data[i][j];
        }

        size_type size() const { return m_size; }

    private:
        data_type m_data{};
        size_type m_size{};
    };

    template <std::floating_point Num>
#ifdef USE_1D_REPRESENTATION
    using Matrix = Matrix_1D<Num>;
#else
    using Matrix = Matrix_2D<Num>;
#endif

    using VectorD = Vector<double>;
    using MatrixD = Matrix<double>;

    template <typename T>
    void printVector(const Vector<T> &v) {
        fmt::print("{}\n", v);
    }

    template <typename T>
    void printMatrix(const Matrix<T> &m) {
        for (auto row = 0u; row < m.size(); row++) {
            for (auto col = 0u; col < m.size(); col++) {
                fmt::print("{} ", m(row, col));
            }
            fmt::print("\n");
        }
    }

    template <Number T>
    auto generateMatrix(std::size_t size, MatrixType matrixType = MatrixType::NORMAL, unsigned seed = 42) -> Matrix<T> {

        using Constants::MIN;
        using Constants::MAX;

        std::random_device rd;
        std::default_random_engine eng(rd());
        eng.seed(seed);

        Matrix<T> generatedMatrix(size);

        auto genMatrix = [&generatedMatrix, &eng, size, &matrixType](auto distr) -> void {
            switch (matrixType) {
                case MatrixType::NORMAL: {
                    for (std::size_t i = 0; i < size; i++) {
                        for (std::size_t j = 0; j < size; j++) {
                            generatedMatrix(i, j) = distr(eng);
                        }
                    }
                    break;
                }
                case MatrixType::DIAGONAL: {

                    for (std::size_t i = 0; i < size; i++) {
                        for (std::size_t j = 0; j < i + 1; j++) {
                            generatedMatrix(i, j) = distr(eng);
                        }
                    }
                    break;
                }
                case MatrixType::DIAGONALLY_DOMINANT: {
                    for (std::size_t i = 0; i < size; i++) {

                        auto sum{ 0.0 };

                        for (std::size_t j = 0; j < size; j++) {
                            if (j != i) {
                                auto cell = distr(eng);
                                generatedMatrix(i, j) = cell;
                                sum = sum + cell;
                            }
                        }

                        generatedMatrix(i, i) = sum + distr(eng);
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

        using Constants::MIN;
        using Constants::MAX;

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
