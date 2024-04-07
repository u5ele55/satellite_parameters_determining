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
    int stateSize = q.size();
    Matrix AT_Kinv(stateSize, measurements[0].size());
    Matrix AT_Kinv_A(stateSize, stateSize);
    Matrix firstSum(stateSize, stateSize);
    Vector secondSum(stateSize);

    for(int k = 0; k < N; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
        if (A.size().first != 3){
            throw std::runtime_error("Got too far from true state");
        }

        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i] / diagonalK[j];
            }
        }
        AT_Kinv_A = AT_Kinv * A;
        if (k == 1) std::cout << "invertible? " << LinAlg::matrixDeterminant(AT_Kinv_A) << '\n';
        firstSum += AT_Kinv_A;
        Vector delta_r = measurements[k] - (*desFunctions[k])(q);
        secondSum += AT_Kinv * delta_r;
        // std::cout << "q: " <<  q << '\n';
    }
    std::cout << "fs invertible? " << LinAlg::matrixDeterminant(firstSum) << '\n';
    auto fsInv = firstSum;
    LinAlg::naiveInverse(fsInv);
    std::cout << firstSum << "\n";
    std::cout << secondSum << "\n";
    q = q + fsInv * secondSum;


    return q;
}
