#include "DesignationsNoiseApplier.hpp"


Vector DesignationsNoiseApplier::targetTelescope(const Vector &ecef)
{
    auto designations = radioControl.targetTelescope(ecef);
    
    if (designations.size() == 3) {
        designations[0] += distanceNoise(gen);
        designations[1] += angleNoise(gen);
        designations[2] += angleNoise(gen);
    }
    
    return designations;
}

DesignationsNoiseApplier::DesignationsNoiseApplier(TelescopeControl &radioControl, double distanceMSE, double angleMSE)
    : radioControl(radioControl),
    gen(rd()),
    distanceNoise(0, distanceMSE),
    angleNoise(0, angleMSE)
{}
