#include "TelescopeControl.hpp"

#include "global/Constants.hpp"
#include "LinAlg.hpp"
#include "coordinates.hpp"
#include "global/Constants.hpp"
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
    
    auto local = convertToLocalSK(ecef);
    // return local;
    
        double dist = calculateDistance(local);
        double az = calculateAzimuth(local);
        double gamma = calculateTargetAngle(local);

            return {dist, az, gamma};
    
    return {}; 
}

Vector TelescopeControl::convertToLocalSK(Vector r_sat)
{
    auto rtBLH = telescope.getBLH();
    double B = rtBLH[0], L = rtBLH[1];
    double cosB = cos(B), sinB = sin(B);
    double cosL = cos(L), sinL = sin(L);
    Matrix R = Matrix{
        {-cosL*sinB,  -sinL*sinB,   cosB},
        { cosL*cosB,   sinL*cosB,   sinB},
        {-sinL,        cosL,        0}
    };
    Vector UEN = R * (r_sat - rtCoord);

    return UEN;

    // auto E = LinAlg::Identity(3);
    // Vector x_new = E[0], y_new = E[1], z_new = E[2];

    // z_new = LinAlg::rotateAbout(z_new, x_new, -(rtBLH[0] - M_PI_2));
    // y_new = LinAlg::rotateAbout(y_new, x_new, -(rtBLH[0] - M_PI_2));
    
    // double angle = LinAlg::angle(-rtCoord + Vector{0,0,Constants::Earth::MINOR_AXIS}, x_new);

    // x_new = LinAlg::rotateAbout(x_new, z_new, -angle);
    // y_new = LinAlg::rotateAbout(y_new, z_new, -angle);

    // double st_x = r_sat.dot(x_new),
    //        st_y = r_sat.dot(y_new),
    //        st_z = r_sat.dot(z_new);

    // return {st_x, st_z, st_y};
}

double TelescopeControl::calculateAzimuth(Vector r_sat)
{
    double x = r_sat[0], y = r_sat[1], z = r_sat[2];
    
    double azimuth;

    if (fabs(x) < 1e-10) {
        if (z > 0)
            azimuth = M_PI_2;
        else 
            azimuth = M_PI; // mb 3 * M_PI_2
    }
    else {
        azimuth = atan2(z, x);
        if (azimuth < 0) {
            azimuth = 2*M_PI + azimuth;
        }
    }

    return azimuth;
}

double TelescopeControl::calculateTargetAngle(Vector r_sat)
{
    double x = r_sat[0], y = r_sat[1], z = r_sat[2];
    return atan2(y, sqrt(x*x + z*z));
}

double TelescopeControl::calculateDistance(Vector r_sat)
{
    return r_sat.norm();
}
