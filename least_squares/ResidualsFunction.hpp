#pragma once

#include "Vector.hpp"
#include "Function/Function.hpp"
#include "global/TaskParameters.hpp"
#include "integration/solver/AbstractSolver.hpp"

class ResidualsFunction : public Function<const Vector&, Vector>
{
private:
    Vector measurement;
    double time;
    TaskParameters *params;
public:
    ResidualsFunction(Vector measurement, double time, TaskParameters *params);

    Vector operator()(const Vector& x) override;
};