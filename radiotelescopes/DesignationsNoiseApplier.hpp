#pragma once

#include <random>

#include "RadioTelescopeSystem.hpp"

class DesignationsNoiseApplier
{
private:
    std::random_device rd; 
    std::mt19937 gen; 
    std::normal_distribution<double> distanceNoise;
    std::normal_distribution<double> angleNoise; 
    RadioTelescopeSystem &radioSystem;
public:
    DesignationsNoiseApplier(RadioTelescopeSystem &radioSystem, double distanceMSE, double angleMSE);
    std::vector<Vector> targetTelescopes(const Vector& ecef);
    ~DesignationsNoiseApplier();
};