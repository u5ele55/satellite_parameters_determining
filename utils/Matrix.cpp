#include "Matrix.hpp"

Matrix::Matrix(int rows, int cols)
    : nm(std::make_pair(rows, cols))
{
    for(int i = 0; i < rows; i ++) {
        this->rows.push_back(Vector(cols));
    }
}

Matrix::Matrix(std::pair<int, int> nm)
    : Matrix(nm.first, nm.second)
{}

Vector &Matrix::at(int y)
{
    return rows[y];
}

Vector Matrix::at(int y) const
{
    return rows[y];
}

Vector &Matrix::operator[](int n) {
    return at(n);
}

Vector Matrix::operator[](int n) const {
    return at(n);
}

std::pair<int, int> Matrix::size() const
{
    return nm;
}

Matrix Matrix::operator+(const Matrix &other) const
{
    if(nm != other.size()) {
        throw std::runtime_error("Matrix::operator+: sizes do not match");
    }
    Matrix result(nm);
    for (int i = 0; i < nm.first; i ++) {
        result[i] = at(i) + other[i];
    }
    return result;
}

Matrix Matrix::operator-() const
{
    Matrix result(nm);
    for (int i = 0; i < nm.first; i ++) {
        result[i] = -at(i);
    }
    return result;
}

Matrix Matrix::operator-(const Matrix &other) const
{
    return *this + (-other);
}

Matrix Matrix::operator*(double val) const
{
    Matrix result(nm);
    for (int i = 0; i < nm.first; i ++) {
        result[i] = at(i) * val;
    }
    return result;
}

Vector Matrix::operator*(const Vector &vec) const
{
    if(nm.second != vec.size()) {
        throw std::runtime_error("Matrix::operator*(Vector): sizes do not match");
    }
    Vector result(vec.size());
    for (int i = 0; i < nm.first; i ++) {
        result[i] = at(i).dot(vec);
    }
    return result;
}

Matrix Matrix::operator*(const Matrix &mtx) const
{
    if(nm.second != mtx.size().first) {
        throw std::runtime_error("Matrix::operator*(Matrix): sizes do not match");
    }
    int n = nm.first, m = mtx.size().second;
    Matrix result(n, m);
    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < m; j ++) {
            result[i][j] += at(i)[j] * mtx[i][j];
        }
    }
    return result;
}
