#pragma once
#include "Vector.hpp"

class Matrix
{
private:
    std::pair<int, int> nm;
    std::vector<Vector> rows;    
public:
    Matrix(int rows, int cols);
    Matrix(std::pair<int, int> nm);
    Matrix(const Matrix& mtx);
    Matrix& operator=(const Matrix& mtx);

    Matrix transposed() const;

    void emplaceColumn(const Vector &column, int index);

    Vector &at(int y);
    Vector at(int y) const;

    Vector &operator[](int n);
    Vector operator[](int n) const;

    std::pair<int, int> size() const;

    Matrix operator+(const Matrix &other) const;

    Matrix operator-() const;

    Matrix operator-(const Matrix &other) const;

    Matrix operator*(double val) const;

    Vector operator*(const Vector &vec) const;

    Matrix operator*(const Matrix &mtx) const;

    friend std::ostream &operator<<(std::ostream &stream, const Matrix &mtx);
};