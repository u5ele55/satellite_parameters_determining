#include "DesignationsNoiseApplier.hpp"

DesignationsNoiseApplier::DesignationsNoiseApplier(RadioTelescopeSystem &radioSystem, double distanceMSE, double angleMSE)
    : radioSystem(radioSystem),
    gen(rd()),
    distanceNoise(0, distanceMSE),
    angleNoise(0, angleMSE)
{}

std::vector<Vector> DesignationsNoiseApplier::targetTelescopes(const Vector &ecef)
{
    auto designations = radioSystem.targetTelescopes(ecef);
    for (int i = 0; i < designations.size(); i ++) {
        designations[i][1] += distanceNoise(gen);
        designations[i][2] += angleNoise(gen);
        designations[i][3] += angleNoise(gen);
    }
    return designations;
}
