#pragma once

#include "Function/Function.hpp"
#include "Vector.hpp"

class F : public Function<double, Vector>
{
private: 
    Vector params;
public:
    F(Vector params) : params(params) {};
    Vector operator()(double time) {
        Vector nl = params;
        nl *= time;
        // nl[0] += time*time*params[4];
        // nl[2] += 1/(time*time + 1)*params[3];
        return nl;
    };
};