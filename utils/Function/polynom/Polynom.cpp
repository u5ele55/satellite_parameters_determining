#include "Polynom.hpp"

Polynom::Polynom(int degree, std::vector<double> coefs)
:   deg(degree),
    coefs(coefs)
{}

double Polynom::operator()(double x) {
    double res = 0, xn = 1;
    for(int i = 0; i <= deg; i ++) {
        res += xn * coefs[i];
        xn *= x;
    }
    return res;
}