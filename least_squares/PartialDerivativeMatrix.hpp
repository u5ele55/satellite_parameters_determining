#pragma once
#include "Matrix.hpp"
#include "Function/Function.hpp"

class PartialDerivativeMatrix
{
private:
    Vector startPoint;
    Vector steps;
    Function<Vector, const Vector&>* f;
public:
    PartialDerivativeMatrix(Function<Vector, const Vector&>* f, Vector startPoint, Vector steps);
    Matrix getMatrix();
    void setStartPoint(Vector startPoint);
};
