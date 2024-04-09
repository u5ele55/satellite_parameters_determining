#include "Iterator.hpp"

#include "DesignationFunctionGenerator.hpp"
#include "PartialDerivativeMatrix.hpp"

#include "LinAlg.hpp"

#include "core/TestFunctionGenerator.hpp"

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
    TestFunctionGenerator tfg(times);
    desFunctions = tfg.generate();
    // diagonalK[0] = params->distMSE * params->distMSE;
    // diagonalK[1] = params->angleMSE * params->angleMSE;
    // diagonalK[2] = params->angleMSE * params->angleMSE;
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
    Vector steps = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    int stateSize = q.size();
    int measurementSize = measurements[0].size();
    Matrix AT_Kinv(stateSize, measurementSize);
    Matrix AT_Kinv_A(stateSize, stateSize);
    Matrix firstSum(stateSize, stateSize);
    Vector secondSum(stateSize);

    Matrix bigA(N * 1, stateSize);
    Vector bigR(N * 1);

    for(int k = 0; k < N; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
        if (A.size().first != measurementSize){
            std::cout << "fallen at " << k << ": " << measurements[k] << ", got " << A << '\n';
            throw std::runtime_error("Got too far from true state");
        }
        Vector delta_r = measurements[k] - (*desFunctions[k])(q);
        for (int i = 0; i < 1; i ++) {
            bigA[k * 1 + i] = A[i];
            bigR[k * 1 + i] = delta_r[i];
        }
    }

    Matrix bigATK(bigA.size().second, bigA.size().first);

    for(int i = 0; i < bigATK.size().first; i ++) {
        for(int j = 0; j < bigATK.size().second; j ++) {
            bigATK[i][j] = bigA[j][i];
        }
    }
    std::cout << "bigA : " << bigA << '\n';
    std::cout << "bigATK : " << bigATK << '\n';
    std::cout << "bigR : " << bigR << '\n';
    auto bigATKA = bigATK * bigA;
    std::cout << "bigATKA : " << bigATKA << '\n';
    std::cout << "fs invertible? " << LinAlg::matrixDeterminant(bigATKA) << '\n';
    auto fsInv = bigATKA;
    LinAlg::naiveInverse(fsInv);

    secondSum = bigATK * bigR;

    q = q + fsInv * secondSum;

    return q;
}
