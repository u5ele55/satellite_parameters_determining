#include "PartialDerivativeMatrix.hpp"

PartialDerivativeMatrix::PartialDerivativeMatrix(
    Function<const Vector&, Vector> *f, 
    Vector startPoint, 
    Vector steps
)   : f(f),
    startPoint(startPoint),
    steps(steps)
{}
#include <iostream>
Matrix PartialDerivativeMatrix::getMatrix()
{
    Vector startF = (*f)(startPoint);
    
    Matrix A(startF.size(), startPoint.size());

    Vector shiftedPoint(startPoint.size());
    for (int i = 0; i < steps.size(); i ++) {
        shiftedPoint = startPoint;
        shiftedPoint[i] += steps[i];
        Vector shiftedF = (*f)(shiftedPoint);
        Vector derivative = (shiftedF - startF) / steps[i];
        for(int j = 0; j < derivative.size(); j ++) {
            A[j][i] = derivative[j];
        }
    }
    return A;
}

void PartialDerivativeMatrix::setStartPoint(Vector startPoint)
{
    this->startPoint = startPoint;
}
