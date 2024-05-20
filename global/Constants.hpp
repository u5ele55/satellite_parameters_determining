#pragma once

#include <cmath>

namespace Constants
{
    namespace Earth
    {
        // ПЗ - 90.11
        const double ANGULAR_SPEED = 7.292115e-5; // rad/sec
        const double MASS = 5.9742e24;
        const double GEOCENTRIC_GRAVITATION_CONSTANT = 398600.44; 
        const double MAJOR_AXIS = 6378.136; // in km
        const double MINOR_AXIS = 6356.7523142; // in km
        const double RADIUS = 6371; // in km, radial radius
        const double RADIUS_E = 6378.140;  // in km, equatorial radius
        const double ALPHA_E = 1.0/298.257222101; // Earth polar compression coefficient
        const double SECONDS_IN_DAY = 86400;
    } // namespace Earth
    namespace Common
    {
        const double G = 6.67430e-20; // km^3 sec^-2 kg^-1
        const double R = 287.05287;
    } // namespace Common
    namespace Satellite
    {
        const double Sb = 0.001;
    } // namespace Satellite
    
} // namespace Constants