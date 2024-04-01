#include "Iterator.hpp"

#include "ResidualsFunctionGenerator.hpp"
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
    ResidualsFunctionGenerator resGen(measurements, times, params);
    resFunctions = resGen.generate();
    diagonalK[0] = params->distMSE * params->distMSE;
    diagonalK[1] = params->angleMSE * params->angleMSE;
    diagonalK[2] = params->angleMSE * params->angleMSE;
}

Iterator::~Iterator()
{
    for (auto *res : resFunctions)
        delete res;
}
#include <iostream>
Vector Iterator::makeIteration()
{
    int N = measurements.size();
    Vector steps = {0.5, 1, 0.5, 1, 0.5, 1};

    Matrix AT_Kinv(q.size(), measurements[0].size());
    Matrix firstSum(q.size(), q.size());
    Vector secondSum(q.size());

    for(int i = 0; i < N; i ++) {
        PartialDerivativeMatrix genA(resFunctions[i], q, steps);
        Matrix A = genA.getMatrix();
        std::cout << "A " << A << '\n';
        for(int i = 0; i < AT_Kinv.size().first; i ++) {
            for(int j = 0; j < AT_Kinv.size().second; j ++) {
                AT_Kinv[i][j] = A[j][i] / diagonalK[j];
            }
        }
        std::cout << "ATK*A" << AT_Kinv * A << '\n';
        auto L = LinAlg::choleskyDecomposition(AT_Kinv * A);
        std::cout << "L LT " << L * L.transposed() << '\n';
    }

    return q;
}
