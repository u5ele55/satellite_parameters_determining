#pragma once

#include "Function/Function.hpp"
#include "Vector.hpp"
#include "global/TaskParameters.hpp"

class StateFunctionGenerator
{
private:
    std::vector<Vector> measurements;
    std::vector<double> times;
    TaskParameters *params;
public:
    StateFunctionGenerator(std::vector<double> times, TaskParameters *params);

    std::vector<Function<const Vector &, Vector>*> generate();
};

