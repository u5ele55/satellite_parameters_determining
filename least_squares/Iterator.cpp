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
    if (diagonalK.size() != measurements[0].size()) {
        throw std::runtime_error("Covarience matrix has inappropriate size");
    }
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
    // std::cout << "\t--Iteration log--\n";
    int N = measurements.size();
    double vs = q.subvector(0, 2).norm() * 0.0001, 
           rs = q.subvector(3, 5).norm() * 0.0001;
    Vector steps = {vs,vs,vs, rs,rs,rs};
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
        // skipping measurements where azimuth could have discontinuity 
        if (abs(measurements[k][1]) > 2 * M_PI - 0.5) {
            skippedCnt ++;
            continue;
        }
        if (A.size().first != measurementSize || A.size().second != stateSize){
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
        
        Vector delta_r = measurements[k] - genA.getStartMeasurement();
        secondSum += AT_Kinv * delta_r;
    }
    if (skippedCnt >= N / 2) {
        std::cout << "\tSkipped half of measurements. Quitting. \n";
        return q;
    }
    if (skippedCnt) std::cout << "\tSkipped: " << skippedCnt << '\n';
    // std::cout << "\tfs invertible? " << LinAlg::matrixDeterminant(firstSum) << '\n';
    // std::cout << firstSum << "\n";
    auto fsInv = firstSum;
    LinAlg::naiveInverse(fsInv);
    // std::cout << '\t' << fsInv << '\n';
    // std::cout << '\t' << secondSum << '\n';
    // std::cout << "\t- " << fsInv * secondSum << "\n";
    // std::cout << "\tE : " << fsInv * firstSum << '\n';
    q = q + fsInv * secondSum;

    return q;
}
