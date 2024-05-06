#include "Iterator.hpp"

#include "functions/DesignationFunctionGenerator.hpp"
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
    diagonalK(params->MSEs.size())
{
    DesignationFunctionGenerator desGen(times, params);
    desFunctions = desGen.generate();
    for (int i = 0; i < diagonalK.size(); i ++) {
        diagonalK[i] = pow(params->MSEs[i], 2);
    }
}

Iterator::~Iterator()
{
    for (auto *res : desFunctions)
        delete res;
}
#include <iostream>
Vector Iterator::makeIteration()
{
    std::cout << "\t--Iteration log--\n";
    int N = measurements.size();
    Vector steps = {.5, .5, .5, 10, 10, 10};
    int stateSize = q.size();
    int measurementSize = measurements[0].size();
    Matrix AT_Kinv(stateSize, measurementSize);
    Matrix AT_Kinv_A(stateSize, stateSize);
    Matrix firstSum(stateSize, stateSize);
    Vector secondSum(stateSize);

    int skippedCnt = 0;

    for(int k = 0; k < N; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
        if (A.size().first != measurementSize || A.size().second != stateSize){
            // std::cout << "\tskipping " << k << ": " << measurements[k] << '\n';
            skippedCnt ++;
            continue;
        }
        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i] / diagonalK[j];
            }
        }
        AT_Kinv_A = AT_Kinv * A;
        firstSum += AT_Kinv_A;
        
        Vector delta_r = measurements[k] - (*desFunctions[k])(q);
        secondSum += AT_Kinv * delta_r;
    }
    if (skippedCnt >= N / 2) {
        std::cout << "\tSkipped half of measurements. Quitting. \n";
        return q;
    }
    std::cout << "\tSkipped: " << skippedCnt << '\n';
    std::cout << "\tfs invertible? " << LinAlg::matrixDeterminant(firstSum) << '\n';
    // std::cout << firstSum << "\n";
    auto fsInv = firstSum;
    // std::cout << '\t' << fsInv << '\n';
    std::cout << '\t' << secondSum << '\n';
    LinAlg::naiveInverse(fsInv);
    std::cout << "\t- " << fsInv * secondSum << "\n";
    q = q + fsInv * secondSum;

    return q;
}
