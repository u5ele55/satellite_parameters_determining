#pragma once

#include "ISystem.hpp"

class SpacecraftECI : public ISystem {
public:
    /// @param mu geocentric gravitational constant 
    /// @param omega earth angular speed
    /// @param initialState initial state of SC
    SpacecraftECI(double mu, double omega, const Vector& initialState);

    void f(Vector &state, double time) const;

    Vector getInitialState() const;

    ~SpacecraftECI() {};
private:
    double mu;
    double omega;
    Vector initialState;
};