#pragma once

#include "global/TaskParameters.hpp"
#include "Vector.hpp"

struct Conditions
{
    TaskParameters *parameters;
    std::vector<double> times;
    std::vector<Vector> measurements;
};
