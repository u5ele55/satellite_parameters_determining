#pragma once

#include "Vector.hpp"
#include "Function/Function.hpp"
#include "global/TaskParameters.hpp"
#include "integration/solver/AbstractSolver.hpp"

class ResidualsFunction : public Function<Vector, const Vector&>
{
private:
    std::vector<Vector> measurements;
    std::vector<double> times;
    TaskParameters params;
public:
    ResidualsFunction(std::vector<Vector> measurements, std::vector<double> times, TaskParameters params);

    Vector operator()(const Vector& x);
};