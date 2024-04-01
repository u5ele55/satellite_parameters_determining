#pragma once

#include "global/TaskParameters.hpp"
#include "Function/Function.hpp"

class DesignationFunction : public Function<const Vector&, Vector>
{
private:
    double time;
    TaskParameters *params;
public:
    DesignationFunction(double time, TaskParameters *params);
    Vector operator()(const Vector& x) override;
};
