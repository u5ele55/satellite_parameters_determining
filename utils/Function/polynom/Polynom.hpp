#pragma once

#include "Function/Function.hpp"
#include <vector>

class Polynom : public Function<double, double>
{
private:
    int deg;
    std::vector<double> coefs;
public:
    /// @brief Polynom with `degree` degree
    /// @param degree Degree of a polynom
    /// @param coefs Vector of size (`degree` + 1): [a_0, a_1, ..., a_degree]
    Polynom(int degree, std::vector<double> coefs);
    double operator()(double x);
};