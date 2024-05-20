#pragma once

#include "Function/Function.hpp"
#include "Vector.hpp"
#include "global/TaskParameters.hpp"

class DesignationFunctionGenerator
{
private:
    std::vector<double> times;
    TaskParameters *params;
public:
    DesignationFunctionGenerator(std::vector<double> times, TaskParameters *params);

    std::vector<Function<const Vector &, Vector>*> generate();
};
