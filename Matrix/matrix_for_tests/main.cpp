#include <iostream>
#include <algorithm>
#include <vector>

template<size_t N, size_t M, typename T = int64_t>
class Matrix {
public:
    Matrix() {
        matrix_.assign(N, std::vector<T>(M, T(0)));
    }
    Matrix(const std::vector<std::vector<T>>& matrix) : matrix_(matrix) {};
    Matrix(T elem) {
        matrix_.assign(N, std::vector<T>(M, T(elem)));
    }
    Matrix(const Matrix<N, M, T>& other) = default;
    Matrix<N, M, T>& operator=(const Matrix<N, M, T>& other) {
        matrix_ = other.matrix_;
        return *this;
    }
    Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& other) {
        for(size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] += other.matrix_[i][j];
            }
        }
        return *this;
    }
    Matrix<N, M, T> operator+(const Matrix<N, M, T>& other) {
        Matrix<N, M, T> copy = *this;
        copy += other;
        return copy;
    }
    Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& other) {
        for(size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] -= other.matrix_[i][j];
            }
        }
        return *this;
    }
    Matrix<N, M, T> operator-(const Matrix<N, M, T>& other) {
        Matrix<N, M, T> copy = *this;
        copy -= other;
        return copy;
    }
    Matrix<N, M, T>& operator*=(const T& value) {
        for(size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] *= value;
            }
        }
        return *this;
    }
    Matrix<N, M, T> operator*(const T& value) {
        Matrix<N, M, T> copy = *this;
        copy *= value;
        return copy;
    }
    template<size_t U>
    Matrix<N, U, T> operator*(const Matrix<M, U, T>& other) {
        Matrix<N, U, T> answer;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < U; ++j) {
                T result = T(0);
                for (size_t k; k < M; ++k) {
                    result += matrix_[i][k] * other.matrix_[k][j];
                }
                answer[i][j] = result;
            }
        }
        return answer;
    }
    Matrix<M, N, T> Transposed() const {
        Matrix<M, N, T> answer;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                answer.matrix_[i][j] = matrix_[j][i];
            }
        }
        return answer;
    }
    T Trace() const {
        Matrix<N, N, T> copy = *this;
        T answer = T(0);
        for (size_t i = 0; i < N; ++i) {
            answer += copy.matrix_[i][i];
        }
        return answer;
    }
    T& operator()(size_t row, size_t column) {
        return matrix_[row][column];
    }
    const T& operator()(size_t row, size_t column) const {
        return matrix_[row][column];
    }
    bool operator==(const Matrix<N, M, T>& other) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if (matrix_[i][j] != other.matrix_[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
private:
    std::vector<std::vector<T>> matrix_;
};

int main() {
    int64_t a;
    Matrix<2, 3, int> M(5);
    //M.Print();
    std::cout << M.Trace() << std::endl;
    //M.Print();
    return 0;
}
