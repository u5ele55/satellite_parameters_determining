#include "PartialDerivativeMatrix.hpp"

PartialDerivativeMatrix::PartialDerivativeMatrix(
    Function<const Vector&, Vector> *f, 
    Vector startPoint, 
    Vector steps
)   : f(f),
    startPoint(startPoint),
    startF(0),
    steps(steps)
{}
#include <iostream>
Matrix PartialDerivativeMatrix::getMatrix()
{
    startF = (*f)(startPoint);
    Vector shiftedF(startF.size());
    Matrix A(startF.size(), startPoint.size());

    Vector shiftedPoint(startPoint.size());
    for (int i = 0; i < steps.size(); i ++) {
        shiftedPoint = startPoint;
        shiftedPoint[i] += steps[i];
        // std::cout << "shiftedPoint: " << shiftedPoint << '\n';
        shiftedF = (*f)(shiftedPoint);
        Vector derivative = (shiftedF - startF) / steps[i];
        // std::cout << "derivative: " << derivative << '\n';
        for(int j = 0; j < derivative.size(); j ++) {
            A[j][i] = derivative[j];
        }
    }
    
    return A;
}

Vector PartialDerivativeMatrix::getStartMeasurement() const
{
    return startF;
}

void PartialDerivativeMatrix::setStartPoint(Vector startPoint)
{
    this->startPoint = startPoint;
}
