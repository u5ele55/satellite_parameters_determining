#pragma once

#include <random>

#include "TelescopeControl.hpp"

class DesignationsNoiseApplier
{
private:
    std::random_device rd; 
    std::mt19937 gen; 
    std::normal_distribution<double> distanceNoise;
    std::normal_distribution<double> angleNoise; 
    TelescopeControl &radioControl;
public:
    DesignationsNoiseApplier(TelescopeControl &radioControl, double distanceMSE, double angleMSE, int seed = 0);
    Vector targetTelescope(const Vector& ecef);
};