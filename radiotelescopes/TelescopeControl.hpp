#pragma once
#include "RadioTelescope.hpp"

class TelescopeControl
{
private:
    RadioTelescope telescope;
    Vector rtCoord;
    Vector planeNormal;
    bool convertToKm;
    bool convertToDegrees;
public:
    TelescopeControl(
        RadioTelescope telescope,
        bool convertToKm = false, 
        bool convertToDegrees = false);
    /// @brief Gives target to a telescope
    /// @param ecef ECEF of satellite
    /// @param index Index of station
    /// @return Empty vector if telescope isn't in RVZ. 
    /// Vector, that consists of one element (0) if telescope in RVZ.
    /// Vector of 3 components with designations if telescope can target satellite.
    /// Components are [distance, azimuth, angle]
    Vector targetTelescope(const Vector& ecef);
private:
    double calculateAzimuth(Vector r_sat, Vector r_st);
};