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
    int measurementSize = measurements[0].size();
    Matrix AT_Kinv(stateSize, measurementSize);
    Matrix AT_Kinv_A(stateSize, stateSize);
    Matrix firstSum(stateSize, stateSize);
    Vector secondSum(stateSize);

    for(int k = 2; k < N-2; k ++) {
        PartialDerivativeMatrix genA(desFunctions[k], q, steps);
        Matrix A = genA.getMatrix();
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
    std::cout << '\t' << fsInv << '\n';
    std::cout << '\t' << secondSum << '\n';
    LinAlg::naiveInverse(fsInv);
    std::cout << "\t- " << fsInv * secondSum << "\n";
    q = q + fsInv * secondSum;


    return q;
}
