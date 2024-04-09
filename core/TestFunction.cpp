#include "TestFunction.hpp"

#include "F.hpp"
#include "math.h"

Vector TestFunction::operator()(const Vector &arg)
{
    auto f = F(arg);
    auto val = f(time);
    // int len = 6;

    // Vector designation(len);
    // for(int i = 0; i < len; i ++) {
    //     designation[i] = val[i]*val[i] + val[val.size()-1-i];
    // }
    Vector designation(2);
    designation = {val.dot(val) - val[0]*val[1], 3*pow(val[0], 1.1) + 5*pow(val[3], 1.2)};
    return designation;
}