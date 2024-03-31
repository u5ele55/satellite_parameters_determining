#pragma once
#include "Matrix.hpp"
#include "Function/Function.hpp"

class PartialDerivativeMatrix
{
private:
    Vector startPoint;
    Vector steps;
    Function<const Vector&, Vector>* f;
public:
    PartialDerivativeMatrix(Function<const Vector&, Vector>* f, Vector startPoint, Vector steps);
    Matrix getMatrix();
    void setStartPoint(Vector startPoint);
};
