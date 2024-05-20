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

Matrix::Matrix(const std::initializer_list<Vector> &lst)
{
    nm = std::make_pair(lst.size(), lst.begin()->size());
    for (const auto &v : lst) {
        this->rows.push_back(Vector(v));
    }
}

Matrix::Matrix(const Matrix &mtx)
    : nm(mtx.nm)
{
    for(int i = 0; i < nm.first; i ++) {
        this->rows.push_back(Vector(mtx[i]));
    }
}

Matrix &Matrix::operator=(const Matrix &mtx)
{
    nm = mtx.nm;
    this->rows = mtx.rows;
    return *this;
}

Matrix Matrix::transposed() const
{
    Matrix result(nm.second, nm.first);

    for(int i = 0; i < nm.first; i ++) {
        for(int j = 0; j < nm.second; j ++) {
            result[j][i] = at(i)[j];
        }
    }
    return result;
}

void Matrix::emplaceColumn(const Vector &column, int index)
{
    if (column.size() != nm.first || index >= nm.second || index < 0) {
        throw std::invalid_argument("Matrix::emplaceColumn: Wrong sizes");
    }
    for (int i = 0; i < nm.first; i++) {
        at(i)[index] = column[i];
    }
}

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
    Vector result(nm.first);
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
            for (int k = 0; k < nm.second; k ++)
                result[i][j] += at(i)[k] * mtx[k][j];
        }
    }
    return result;
}

Matrix &Matrix::operator+=(const Matrix &mtx)
{
    if(nm != mtx.size()) {
        throw std::runtime_error("Matrix::operator+=: sizes do not match");
    }

    for (int i = 0; i < nm.first; i ++) {
        rows[i] += mtx[i];
    }

    return *this;
}

std::ostream &operator<<(std::ostream &stream, const Matrix &mtx)
{
    stream << "[\n";
    for (int i = 0; i < mtx.nm.first; i++) {
        stream << '\t' << mtx.at(i) << '\n';
    }
    stream << "]";

    return stream;
}
