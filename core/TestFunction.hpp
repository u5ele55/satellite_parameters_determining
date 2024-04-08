
#pragma once

#include "Function/Function.hpp"

#include "Vector.hpp"

class TestFunction : public Function<const Vector&, Vector> 
{
private:
    double time;
public:
    TestFunction(double time) : time(time) {};
    Vector operator()(const Vector& arg) override;
};
