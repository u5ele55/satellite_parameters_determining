#include "TelescopeControl.hpp"

#include "global/Constants.hpp"
#include "coordinates.hpp"
#include <cmath>

TelescopeControl::TelescopeControl(
    RadioTelescope telescope, 
    bool convertToKm, 
    bool convertToDegrees
) : telescope(telescope),
    rtCoord(blh2ecef(telescope.getBLH())),
    convertToDegrees(convertToDegrees),
    convertToKm(convertToKm),
    planeNormal(3)
{
    double a = Constants::Earth::MAJOR_AXIS;
    double b = Constants::Earth::MINOR_AXIS;
    planeNormal = {rtCoord[0] / (a*a), rtCoord[1] / (a*a), rtCoord[2] / (b*b)};
    planeNormal /= planeNormal.norm();
}

Vector TelescopeControl::targetTelescope(const Vector &ecef)
{
    // simplicified to sphere
    double maxDistanceSqr = ecef.dot(ecef) - Constants::Earth::MINOR_AXIS * Constants::Earth::MINOR_AXIS;

    auto r = ecef - rtCoord;
    double distanceSqr = r.dot(r);

    if (distanceSqr < maxDistanceSqr) {
        // find angle between RT plane and vector between RT and satellite
        double cosAngle = r.dot(planeNormal) / r.norm();
        if (cosAngle < 0) {
            return {};
        }
        // acos is an angle between vector and plane
        double angle = M_PI_2 - acos(cosAngle);
        
        if (angle > telescope.getAngle()) {
            // could be seen!
            double azimuth = calculateAzimuth(ecef, rtCoord); 
            double distance = sqrt(distanceSqr); // in meters
            return {
                convertToKm ? std::round(distance) / 1000.0 : distance,
                convertToDegrees ? azimuth * 180 / M_PI : azimuth,
                convertToDegrees ? angle * 180 / M_PI : angle 
            };
        } else {
            // just in radiozone
            return {0};
        }
    }
    return {}; //
}

double TelescopeControl::calculateAzimuth(Vector r_sat, Vector r_st)
{
    double k = planeNormal.dot(r_sat - r_st);

    // Projection of satellite on a station plane
    auto l_p = r_sat - planeNormal*k;
    Vector north(3);

    if (r_st[2] == 0) {
        north = {0,0,1};
    } else if (r_st[2] < 0) {
        double z_north_intersect = r_st.dot(r_st) / r_st[2];
        north = r_st - Vector{0, 0, z_north_intersect};
    } else {
        double z_north_intersect = r_st.dot(r_st) / r_st[2];
        north = Vector{0, 0, z_north_intersect} - r_st;
    }

    double cosAzimuth = l_p[2] / sqrt(l_p.dot(l_p));

    auto l_pCrossNorth = l_p.cross(north);
    double cosNormCross = l_pCrossNorth.dot(planeNormal) / sqrt(l_pCrossNorth.dot(l_pCrossNorth));

    double azimuth = cosNormCross > 0 ? acos(cosAzimuth) : 2*M_PI - acos(cosAzimuth);

    return azimuth;
}
