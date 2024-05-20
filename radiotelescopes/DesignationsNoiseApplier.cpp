#include "DesignationsNoiseApplier.hpp"

DesignationsNoiseApplier::DesignationsNoiseApplier(TelescopeControl &radioControl, Vector MSEs, int seed)
:   radioControl(radioControl),
    gen(seed == 0 ? rd() : seed)
{
    for (int i = 0; i < MSEs.size(); i ++) {
        noises.emplace_back(0, MSEs[i]);
    }
}
#include <iostream>
Vector DesignationsNoiseApplier::targetTelescope(const Vector &ecef)
{
    auto designations = radioControl.targetTelescope(ecef);
    if (designations.size() == 0) {
        return designations;
    }
    if (designations.size() != noises.size()) {
        std::cout << designations << " " << noises.size() << '\n';
        throw std::runtime_error("DesignationsNoiseApplier::targetTelescope: sizes of designation and noise doesn't match");
    }

    for (int i = 0; i < designations.size(); i ++)
        designations[i] += noises[i](gen);
    
    return designations;
}