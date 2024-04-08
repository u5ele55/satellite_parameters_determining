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

    for(int k = 1; k < N-2; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
        if (A.size().first != measurementSize){
            std::cout << "fallen at " << k << ": " << measurements[k] << ", got " << A << '\n';
            throw std::runtime_error("Got too far from true state");
        }

        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i];
            }
        }
        AT_Kinv_A = AT_Kinv * A;
        if (k == 10) std::cout << "invertible? " << LinAlg::matrixDeterminant(AT_Kinv_A) << A << '\n';
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
