#pragma once

#include "Vector.hpp"
#include "Matrix.hpp"

class LinAlg {
public:
    /// @brief Rotate `src` about `axis` by `angle` radians. Rotation is clockwise from perspective if you look at `axis` end
    static Vector rotateAbout(const Vector& src, const Vector& axis, double angle);
    static double angle(const Vector& a, const Vector& b);
    static Vector projectionOnPlane(const Vector& src, const Vector& basis1, const Vector& basis2);
    static Vector projectionOnPlane(const Vector& src, const Vector& normal);

    static Vector projectionOnEllipse(const Vector& r, double a, double b, double c);

    static void toRad(double& deg);

    static Matrix choleskyDecomposition(const Matrix &A, double epsilon=1e-6);
    
    static Vector solveCholesky(const Matrix &L, const Vector & b);

    static void solveCholesky(const Matrix &L, const Matrix &B, Matrix &answer);

    static int LUPDecompose(Matrix &A, Vector &P, double tol);

    static void LUPInvert(const Matrix &A, const Vector &P, Matrix &IA);

    static double LUPDeterminant(const Matrix &A, const Vector &P);

    static void naiveInverse(Matrix &A);

    static double matrixDeterminant(const Matrix& A);
public:
    static Matrix Identity(int n);

    static Matrix R_y(double angle);

    static Matrix R_z(double angle);
};