#pragma once

#include "RadioTelescope.hpp"
#include "TelescopeControl.hpp"

#include <vector>

class RadioTelescopeSystem {
public:
    RadioTelescopeSystem(
        std::vector<RadioTelescope> radiotelescopes, 
        bool convertToDegrees = false,
        bool convertToKm = false);
    /// @brief Returns designations of telescopes that see satellite
    /// @param ecef Ecef cooedinate of an satellite
    /// @return [index, distance, azimuth, angle]
    std::vector<Vector> targetTelescopes(const Vector& ecef);
private:
    std::vector<TelescopeControl> rdtsControl;
    bool convertToDegrees;
    bool convertToKm;
};