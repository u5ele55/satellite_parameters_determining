#include "SpacecraftECEF.hpp"

#include "global/Constants.hpp"

#include <array>

const std::array<float, 8> Level = {0.0f, 20.0f, 60.0f, 100.0f, 150.0f, 300.0f, 600.0f, 900.0f};
const std::array<float, 7> A = {1.22499f, 8.89096e-2f, 3.09675e-4f, 5.54950e-7f, 2.00328e-9f, 1.91636e-11f, 1.13959e-13f};
const std::array<float, 7> B = {1.10367e-4f, 1.52497e-4f, 3.92270e-5f, 1.42829e-4f, 5.29811e-5f, 2.44074e-5f, 1.43078e-5f};
const std::array<float, 7> C = {-1.75985e-9f, 3.81794e-10f, -8.29881e-10f, 1.31966e-9f, 3.1294e-10f, 7.46767e-11f, 3.01958e-11f};

float density(float H) {
    if (H > Level[7]) {
        return 0.0f;
    }
    if (H < Level[0]) {
        return 3.3e3f;
    }

    int i = 7;
    while (H < Level[i - 1]) {
        i--;
    }

    float dH = (H - Level[i - 1]) * 1e3f;
    return A[i] * std::exp(dH * dH * C[i] - dH * B[i]);
}
const double C20 = -1.082637e-3;

SpacecraftECEF::SpacecraftECEF(double Sb, const Vector &initialState)
    : Sb(Sb),
    mu(Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT),
    omega(Constants::Earth::ANGULAR_SPEED),
    initialState(initialState)
{}

void SpacecraftECEF::f(Vector &state, double time) const
{
    const double k1 = 1.5*mu*C20*pow(Constants::Earth::RADIUS_E, 2);
    double 
        vx = state[0], x = state[3], 
        vy = state[1], y = state[4], 
        vz = state[2], z = state[5];
    double R2 = x*x + y*y + z*z;
    double R = sqrt(R2);
    double sin2psi = pow(z / R, 2);
    double H = R - Constants::Earth::RADIUS_E * (1.0 - Constants::Earth::ALPHA_E * sin2psi);
    double V = state.subvector(0,2).norm();
    double CF = mu / R2 / R;
    double Ro = density(H) * Sb * V / 1000;
    double w0 = k1 / R2 / R2 / R;
    double w1 = w0 * (1.0 - 5 * sin2psi);
    double w2 = pow(omega, 2) + w1 - CF;
    state[0] = w2*x - Ro*vx + 2*omega*vy;             
    state[1] = w2*y - Ro*vy - 2*omega*vx;             
    state[2] =(w1+w0+w0-CF)*z - Ro*vz;
    state[3] = vx;
    state[4] = vy;
    state[5] = vz; 
}

Vector SpacecraftECEF::getInitialState() const
{
    return initialState;
}
