#include "SpacecraftECEF.hpp"

#include "global/Constants.hpp"

SpacecraftECEF::SpacecraftECEF(double Sb, const Vector &initialState)
    : Sb(Sb),
    mu(Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT),
    omega(Constants::Earth::ANGULAR_SPEED),
    initialState(initialState)
{}

void SpacecraftECEF::f(Vector &state, double time) const
{
    double 
        vx = state[0], x = state[3], 
        vy = state[1], y = state[4], 
        vz = state[2], z = state[5];
    double R2 = x*x + y*y + z*z;
    double R = sqrt(R2);
    
    double mlt = -mu / R2 / R;
    state[0] = mlt*x + omega*omega*x + 2*omega*vy;             
    state[1] = mlt*y + omega*omega*y - 2*omega*vx;             
    state[2] = mlt*z;
    state[3] = vx;
    state[4] = vy;
    state[5] = vz; 
}

Vector SpacecraftECEF::getInitialState() const
{
    return initialState;
}
