#pragma once

#include "RadioTelescope.hpp"

#include <vector>

class RadioTelescopeSystem {
public:
    RadioTelescopeSystem(
        std::vector<RadioTelescope> radiotelescopes, 
        bool convertToDegrees = false,
        bool convertToKm = false);
    std::vector<Vector> targetTelescopes(const Vector& ecef);
    /// @brief Gives target to a telescope
    /// @param ecef ECEF of satellite
    /// @param index Index of station
    /// @return Empty vector if telescope isn't in RVZ. 
    /// Vector, that consists of one element (index) if telescope in RVZ.
    /// Vector of 4 components with designations if telescope can target satellite.
    Vector targetTelescope(const Vector& ecef, int index);
private:
    std::vector<RadioTelescope> rdts;
    std::vector<Vector> rdtsECEF;
    bool convertToDegrees;
    bool convertToKm;
private:
    double calculateAzimuth(Vector r_sat, Vector r_st);
};