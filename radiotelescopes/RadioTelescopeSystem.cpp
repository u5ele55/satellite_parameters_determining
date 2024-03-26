#include "RadioTelescopeSystem.hpp"

#include "utils/coordinates.hpp"
#include "global/Constants.hpp"

#include <cmath>
#include <iostream>

RadioTelescopeSystem::RadioTelescopeSystem(
    std::vector<RadioTelescope> radiotelescopes,
    bool convertToDegrees,
    bool convertToKm
)   
:   rdts(radiotelescopes), 
    convertToDegrees(convertToDegrees), 
    convertToKm(convertToKm)
{
    for(int i = 0; i < rdts.size(); i ++)
        rdtsECEF.push_back(blh2ecef(rdts[i].getBLH()));
}

Vector RadioTelescopeSystem::targetTelescope(const Vector& ecef, int index) {
    if (index < 0 || index > rdts.size()) {
        throw std::invalid_argument("incorrect index of station");
    }
    // simplicified to sphere
    double maxDistanceSqr = ecef.dot(ecef) - Constants::Earth::MINOR_AXIS * Constants::Earth::MINOR_AXIS;

    const auto& rtCoord = rdtsECEF[index];
    auto r = ecef - rtCoord;
    double distanceSqr = r.dot(r);

    if (distanceSqr < maxDistanceSqr) {
        // find angle between RT plane and vector between RT and satellite
        double cosAngle = r.dot(rtCoord) / sqrt(r.dot(r) * rtCoord.dot(rtCoord));
        if (cosAngle < 0) {
            return {};
        }
        // acos is an angle between vector and plane
        double angle = M_PI_2 - acos(cosAngle);
        
        if (angle > rdts[index].getAngle()) {
            // could be seen!
            double azimuth = calculateAzimuth(ecef, rtCoord); 
            double distance = sqrt(distanceSqr); // in meters
            return {
                (double)index, 
                convertToKm ? std::round(distance) / 1000.0 : distance,
                convertToDegrees ? azimuth * 180 / M_PI : azimuth,
                convertToDegrees ? angle * 180 / M_PI : angle 
            };
        } else {
            // just in radiozone
            return {(double)index};
        }
    }
    return {}; // 
}

std::vector<Vector> RadioTelescopeSystem::targetTelescopes(const Vector& ecef)
{
    std::vector<Vector> designations; // in format {r_id, dist, azimuth, angle}

    for (int rInd = 0; rInd < rdts.size(); rInd ++) {
        const auto &des = targetTelescope(ecef, rInd);
        if (des.size() != 4) {
            continue;
        }
        designations.push_back(des);
    }
    return designations;
}

double RadioTelescopeSystem::calculateAzimuth(Vector r_sat, Vector r_st)
{
    // Normal vector to station plane
    auto normalVector = r_st * (1 / sqrt(r_st.dot(r_st)));

    double k = normalVector.dot(r_sat - r_st);

    // Projection of satellite on a station plane
    auto l_p = r_sat - normalVector*k;
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
    double cosNormCross = l_pCrossNorth.dot(normalVector) / sqrt(l_pCrossNorth.dot(l_pCrossNorth));

    double azimuth = cosNormCross > 0 ? acos(cosAzimuth) : 2*M_PI - acos(cosAzimuth);

    return azimuth;
}
