#pragma once

#include "Function/Function.hpp"
#include "Vector.hpp"
#include "global/TaskParameters.hpp"

class ResidualsFunctionGenerator
{
private:
    std::vector<Vector> measurements;
    std::vector<double> times;
    TaskParameters params;
public:
    ResidualsFunctionGenerator(std::vector<Vector> measurements, std::vector<double> times, TaskParameters params);

    std::vector<Function<const Vector &, Vector>*> generate();
};

