#include "LinAlg.hpp"

#include <cmath>

// method from https://math.stackexchange.com/questions/511370/how-to-rotate-one-vector-about-another
Vector LinAlg::rotateAbout(const Vector &a, const Vector &b, double angle)
{
    Vector a_prl_b = b * ( a.dot(b) / b.dot(b) );
    Vector a_ort_b = a - a_prl_b;
    if (a_ort_b.norm() == 0) {
        return a;
    }
    Vector w = b.cross(a_ort_b);

    double x1 = cos(angle) / a_ort_b.norm();
    double x2 = sin(angle) / w.norm();

    Vector a_ort_rot_b = (a_ort_b * x1 + w * x2) * a_ort_b.norm();

    Vector a_rot_b = a_ort_rot_b + a_prl_b;

    return a_rot_b;
}

double LinAlg::angle(const Vector &a, const Vector &b)
{
    double normA = a.norm(), normB = b.norm();

    if (normA == 0 || normB == 0) {
        return 0;
    }

    double cos_angle = a.dot(b) / (normA * normB);

    return acos(cos_angle);
}

Vector LinAlg::projectionOnPlane(const Vector &src, const Vector &basis1, const Vector &basis2)
{
    Vector n = basis1.cross(basis2); // normal vector

    return projectionOnPlane(src, n);
}

Vector LinAlg::projectionOnPlane(const Vector &src, const Vector &normal)
{
    Vector n = normal / normal.norm();
    Vector proj = n * src.dot(n);

    return proj;
}

Vector LinAlg::projectionOnEllipse(const Vector &r, double a, double b, double c)
{
    double X = r[0], Y = r[1], Z = r[2];
    double t = 1 / sqrt(X*X / (a*a) + Y*Y / (b*b) + Z*Z / (c*c));

    return r * t;
}

void LinAlg::toRad(double &deg)
{
    deg *= M_PI / 180;
}
#include <iostream>
Matrix LinAlg::choleskyDecomposition(const Matrix &A, double epsilon)
{
    if (A.size().first != A.size().second) {
        throw std::invalid_argument("LinAlg::choleskyDecomposition: Wrong sizes");
    }
    int n = A.size().first;
    auto ans = Matrix(n, n);
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < i + 1; k++) {
            long double sum = 0;
            for (int j = 0; j < k; j++) {
                sum = std::fma(ans[i][j], ans[k][j], sum);
            }
            if (i == k) {
                ans[i][k] = sqrt(A[i][i] + epsilon - sum); // stabilization
            } else {
                ans[i][k] = (A[i][k] - sum) / ans[k][k];
            }
        }
    }
    return ans;
}

Vector LinAlg::solveCholesky(const Matrix &L, const Vector &b)
{
    int n = L.size().first;
    if (n != b.size()) {
        throw std::invalid_argument("LinAlg::solveCholesky(Vector): Wrong sizes");
    }
    auto y = Vector(n);

    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++) {
            sum = std::fma(y[j], L[i][j], sum);
        }
        y[i] = (b[i] - sum) / L[i][i];
    }

    auto answer = Vector(n);
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) {
            sum += answer[j] * L[j][i];
        }
        answer[i] = (y[i] - sum) / L[i][i];
    }

    return answer;
}

void LinAlg::solveCholesky(const Matrix &L, const Matrix &B, Matrix &answer)
{
    int n = L.size().first;
    if (n != B.size().first
        || n != answer.size().first
        || answer.size().second != B.size().second) {
        throw std::invalid_argument("LinAlg::solveCholesky(Matrix): Wrong sizes");
    }

    for (int i = 0; i < B.size().second; i++) {
        Vector b(n);
        for(int j = 0; j < B.size().first; j ++) {
            b[j] = B[j][i];
        }
        answer.emplaceColumn(solveCholesky(L, b), i);
    }
}

int LinAlg::LUPDecompose(Matrix &A, Vector &P, double tol)
{
    int i, j, k, imax; 
    double maxA, absA;
    int N = A.size().first;
    Vector row(N);

    for (i = 0; i <= N; i++)
        P[i] = i; //Unit permutation matrix, P[N] initialized with N

    for (i = 0; i < N; i++) {
        maxA = 0.0;
        imax = i;
        
        for (k = i; k < N; k++) {
            absA = fabs(A[k][i]);
            if (maxA < absA) { 
                maxA = absA;
                imax = k;
            }
        }

        if (maxA < tol) {
            std::cout << maxA << " " << A[imax] << '\n';
            for(int q = i; q < N; q ++) 
                std::cout << A[q][i] << '-';
            return 0; //failure, matrix is degenerate
        }
        if (imax != i) {
            //pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            //pivoting rows of A
            row = A[i];
            A[i] = A[imax];
            A[imax] = row;

            //counting pivots starting from N (for determinant)
            P[N]++;
        }

        for (j = i + 1; j < N; j++) {
            A[j][i] /= A[i][i];

            for (k = i + 1; k < N; k++)
                A[j][k] -= A[j][i] * A[i][k];
        }
    }

    return 1;
}

void LinAlg::LUPInvert(const Matrix &A, const Vector &P, Matrix &IA)
{
    int N = A.size().first;
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            IA[i][j] = P[i] == j ? 1.0 : 0.0;

            for (int k = 0; k < i; k++)
                IA[i][j] -= A[i][k] * IA[k][j];
        }

        for (int i = N - 1; i >= 0; i--) {
            for (int k = i + 1; k < N; k++)
                IA[i][j] -= A[i][k] * IA[k][j];

            IA[i][j] /= A[i][i];
        }
    }
}

double LinAlg::LUPDeterminant(const Matrix &A, const Vector &P)
{
    int N = A.size().first;
    double det = A[0][0];

    for (int i = 1; i < N; i++)
        det *= A[i][i];

    return ((int)P[N] - N) % 2 == 0 ? det : -det;
}

void LinAlg::naiveInverse(Matrix &A)
{
    int N = A.size().first;
    Matrix E = LinAlg::Identity(N);
    double temp;

    for (int k = 0; k < N; k++)
    {
        temp = A[k][k];
 
        for (int j = 0; j < N; j++) {
            A[k][j] /= temp;
            E[k][j] /= temp;
        }
 
        for (int i = k + 1; i < N; i++) {
            temp = A[i][k];
 
            for (int j = 0; j < N; j++) {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
 
    for (int k = N - 1; k > 0; k--) {
        for (int i = k - 1; i >= 0; i--) {
            temp = A[i][k];
 
            for (int j = 0; j < N; j++) {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
 
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = E[i][j];
}

double LinAlg::matrixDeterminant(const Matrix &A)
{
    if (A.size().first != A.size().second) {
        throw std::invalid_argument("LinAlg::matrixDeterminant: Matrix should be square-matrix!");
    }
    Matrix B = A;
    int n = B.size().first;
    //приведение матрицы к верхнетреугольному виду
    for(int step = 0; step < n - 1; step++)
        for(int row = step + 1; row < n; row++)
        {
            double coeff = -B[row][step] / B[step][step]; //метод Гаусса
            for(int col = step; col < n; col++)
                B[row][col] += B[step][col] * coeff;
        }
    //Рассчитать определитель как произведение элементов главной диагонали
    double Det = 1;
    for(int i = 0; i < n; i++)
        Det *= B[i][i];
    return Det;
}

Matrix LinAlg::Identity(int n)
{
    Matrix E(n, n);

    for (int i = 0; i < n; i ++)
        E[i][i] = 1;

    return E;
}
