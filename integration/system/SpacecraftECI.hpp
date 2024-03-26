#pragma once

#include "ISystem.hpp"

class SpacecraftECI : public ISystem {
public:
    /// @param mu geocentric gravitational constant 
    /// @param omega earth angular speed
    /// @param initialPosition initial position of SC
    /// @param initialSpeed initial speed of SC
    SpacecraftECI(double mu, double omega, const Vector& initialPosition, const Vector& initialSpeed);

    void f(Vector &state, double time) const;

    Vector getInitialState() const;

    ~SpacecraftECI() {};
private:
    double mu;
    double omega;
    Vector initialState;
};