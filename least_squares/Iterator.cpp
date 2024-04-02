#include "Iterator.hpp"

#include "DesignationFunctionGenerator.hpp"
#include "PartialDerivativeMatrix.hpp"

#include "LinAlg.hpp"

Iterator::Iterator(
    std::vector<Vector> measurements, 
    std::vector<double> times,
    Vector initialGuess, 
    TaskParameters *params
)   : initialGuess(initialGuess),
    params(params),
    measurements(measurements),
    times(times),
    q(initialGuess),
    diagonalK(3)
{
    DesignationFunctionGenerator desGen(times, params);
    desFunctions = desGen.generate();
    diagonalK[0] = params->distMSE * params->distMSE;
    diagonalK[1] = params->angleMSE * params->angleMSE;
    diagonalK[2] = params->angleMSE * params->angleMSE;
}

Iterator::~Iterator()
{
    for (auto *res : desFunctions)
        delete res;
}
#include <iostream>
Vector Iterator::makeIteration()
{
    int N = measurements.size();
    Vector steps = {5, 5, 5, 100, 100, 100};

    Matrix AT_Kinv(q.size(), measurements[0].size());
    Matrix AT_Kinv_A(q.size(), q.size());
    Matrix AT_Kinv_A_inv(q.size(), q.size());
    Matrix firstSum(q.size(), q.size());
    Vector secondSum(q.size());

    Matrix E = LinAlg::Identity(q.size());
    for(int i = 0; i < N; i ++) {   
        PartialDerivativeMatrix genA(desFunctions[i], q, steps);
        std::cout << "getA\n";
        Matrix A = genA.getMatrix();
        std::cout << "A : " << A << '\n';
        // std::cout << A << '\n';
        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i];/// diagonalK[j];
            }
        }
        AT_Kinv_A = AT_Kinv * A;
        Matrix c(AT_Kinv_A.size());
        for(int i = 0; i < c.size().first; i ++) {
            for(int j = 0; j < c.size().second; j ++) {
                c[i][j] = AT_Kinv_A[i][j];
            }
        }
        AT_Kinv_A_inv = AT_Kinv_A;
        std::cout << "invertible? " << LinAlg::matrixDeterminant(AT_Kinv_A) << '\n';
        // LinAlg::naiveInverse(AT_Kinv_A_inv);
        // std::cout << AT_Kinv_A_inv << '\n';
        // std::cout << LinAlg::matrixDeterminant(AT_Kinv_A * AT_Kinv_A_inv) << '\n';
        auto L = LinAlg::choleskyDecomposition(AT_Kinv_A, 1e-6);
        LinAlg::solveCholesky(L, E, AT_Kinv_A_inv);
        // firstSum = firstSum + AT_Kinv_A_inv;
        Vector delta_r = genA.getShiftedMeasurement() - measurements[i];
        // secondSum = secondSum + AT_Kinv * delta_r;
        q = q - AT_Kinv_A_inv * AT_Kinv * delta_r;
        // std::cout << "q: " <<  q << '\n';
    }


    return q;
}
