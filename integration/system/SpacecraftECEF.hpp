#pragma once

#include "ISystem.hpp"

class SpacecraftECEF : public ISystem {
public:
    /// @param mu geocentric gravitational constant 
    /// @param omega earth angular speed
    /// @param initialState initial state of SC
    SpacecraftECEF(double Sb, const Vector& initialState);

    void f(Vector &state, double time) const;

    Vector getInitialState() const;

    ~SpacecraftECEF() {};
private:
    double mu;
    double Sb;
    double omega;
    Vector initialState;
};