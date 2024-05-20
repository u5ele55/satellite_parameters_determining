#pragma once

#include "global/TaskParameters.hpp"
#include "Function/Function.hpp"

class StateFunction : public Function<const Vector&, Vector>
{
private:
    double time;
    TaskParameters *params;
public:
    StateFunction(double time, TaskParameters *params);
    Vector operator()(const Vector& x_0) override;
};
