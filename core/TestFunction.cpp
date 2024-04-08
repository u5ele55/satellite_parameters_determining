#include "TestFunction.hpp"

#include "F.hpp"
#include "math.h"

Vector TestFunction::operator()(const Vector &arg)
{
    auto f = F(arg);
    auto val = f(time);
    Vector designation = {
        val[0]*val[0] + val[3],
        val[1]*val[1] + val[4],
        val[2]*val[2] + val[5]
        };
    return designation;
}