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
:   convertToDegrees(convertToDegrees), 
    convertToKm(convertToKm)
{
    for(auto &tel : radiotelescopes)
        rdtsControl.push_back(TelescopeControl(tel, convertToKm, convertToDegrees));
}

std::vector<Vector> RadioTelescopeSystem::targetTelescopes(const Vector& ecef)
{
    std::vector<Vector> designations; // in format {r_id, dist, azimuth, angle}

    for (int rInd = 0; rInd < rdtsControl.size(); rInd ++) {
        const auto &des = rdtsControl[rInd].targetTelescope(ecef);
        
        if (des.size() != 4) {
            continue;
        }

        Vector ret(des.size() + 1);
        ret[0] = rInd;
        for(int i = 1; i < des.size() + 1; i ++) {
            ret[i] = des[i-1];
        }

        designations.push_back(ret);
    }
    return designations;
}
