#include "SpacecraftECI.hpp"

#include <cmath>

SpacecraftECI::SpacecraftECI(double mu, double omega, const Vector &initialState)
    : mu(mu), omega(omega), initialState(initialState)
{}

void SpacecraftECI::f(Vector &state, double time) const {
    double 
        vx = state[0], x = state[1], 
        vy = state[2], y = state[3], 
        vz = state[4], z = state[5];
    double r3 = pow(x*x + y*y + z*z, 1.5);

    state[0] = -mu/r3 * x;
    state[1] = vx;

    state[2] = -mu/r3 * y;
    state[3] = vy;

    state[4] = -mu/r3 * z;
    state[5] = vz;
}

// [vx, x, vy, y, vz, z]
Vector SpacecraftECI::getInitialState() const {
    return initialState;
}
