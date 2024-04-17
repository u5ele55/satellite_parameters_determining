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
    int N = measurements.size();
    Vector steps = {.05, .05, .05, 1, 1, 1};
    int stateSize = q.size();
    int measurementSize = measurements[0].size();
    Matrix AT_Kinv(stateSize, measurementSize);
    Matrix AT_Kinv_A(stateSize, stateSize);
    Matrix firstSum(stateSize, stateSize);
    Vector secondSum(stateSize);

    for(int k = 2; k < N-2; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
        if (k == 3) std::cout << "A : " << A << '\n';
        if (A.size().first != measurementSize){
            std::cout << "\tskipping " << k << ": " << measurements[k] << '\n';
            continue;
            throw std::runtime_error("Got too far from true state");
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
        // std::cout << "q: " <<  q << '\n';
    }
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
