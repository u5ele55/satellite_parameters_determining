#pragma once

#include <random>

#include "TelescopeControl.hpp"

class DesignationsNoiseApplier
{
private:
    std::random_device rd; 
    std::mt19937 gen; 
    std::vector<std::normal_distribution<double>> noises;
    TelescopeControl &radioControl;
public:
    DesignationsNoiseApplier(TelescopeControl &radioControl, Vector MSEs, int seed = 0);
    Vector targetTelescope(const Vector& ecef);
};