#include "Iterator.hpp"

#include "functions/DesignationFunctionGenerator.hpp"
#include "functions/StateFunctionGenerator.hpp"
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
    StateFunctionGenerator stGen(times, params);
    stateFunctions = stGen.generate();
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
        // std::cout << "genS cr\n";
        // std::cout << stateFunctions.size() << " " << k << '\n';
        Vector x_i = (*stateFunctions[k])(q);
        PartialDerivativeMatrix genS(stateFunctions[k], q, steps);
        // std::cout << "genD cr\n";
        PartialDerivativeMatrix genD(desFunctions[k], x_i, steps);
        // std::cout << "D cr\n";
        Matrix D = genD.getMatrix();
        // std::cout << "S cr\n";
        Matrix S = genS.getMatrix();

        // std::cout << D << " \nS:" << S << '\n';
        // std::cout << D * S << '\n';
        auto A = D * S;
        if (k == 3) std::cout << "A : " << A << '\n';

        if (A.size().first != measurementSize){
            std::cout << "\tskipping " << k << ": " << measurements[k] << '\n';
            continue;
            // throw std::runtime_error("Got too far from true state");
        }

        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i] / diagonalK[j];
            }
        }
        AT_Kinv_A = AT_Kinv * A;
        firstSum += AT_Kinv_A;
        // std::cout << me
        Vector delta_r = measurements[k] - (*desFunctions[k])(x_i);
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
