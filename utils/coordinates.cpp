#include "coordinates.hpp"
#include "../global/Constants.hpp"
#include "time.hpp"

Vector blh2ecef(const Vector &blh)
{
    double b = blh[0], l = blh[1], h = blh[2];
    double cos_b = cos(b), cos_l = cos(l), sin_b = sin(b), sin_l = sin(l);
    
    double r = Constants::Earth::MINOR_AXIS / Constants::Earth::MAJOR_AXIS;
    double e2 = 1 - r*r;
    double N = Constants::Earth::MAJOR_AXIS / sqrt(1 - e2 * sin_b*sin_b);

    Vector ecef = {
        (N + h) * cos_b * cos_l,
        (N + h) * cos_b * sin_l,
        ((1 - e2)*N + h) * sin_b
    };
    return ecef;
}

Vector ecef2blh(const Vector &ecef)
{
    double p;
    double theta;
    Vector blh(3);
    double a = Constants::Earth::MAJOR_AXIS;
    double b = Constants::Earth::MINOR_AXIS;
    double e2 = (a*a - b*b) / (a*a);
    double ed2 = (a*a - b*b) / (b*b);

    try {
        p = sqrt(ecef[0] * ecef[0] + ecef[1] * ecef[1]);
        theta = atan2(ecef[2] * a, p * b);
        blh[0] = atan2(
            ecef[2] + ed2 * b * pow(sin(theta), 3),
            p - e2  * a * pow(cos(theta), 3)
        );                                          // Beta(Latitude)
        double n = a / sqrt(1.0 - e2 * pow(sin(blh[0]), 2));
        blh[1] = atan2(ecef[1], ecef[0]);          // Lambda(Longitude)
        blh[2] = (p / cos(blh[0])) - n;  // Height
    } catch (...) {
        throw;
    }

    return blh;
}

double greenwichSiderealTime(const Vector &currentTime) {
    double hrs = currentTime[3], mns = currentTime[4], scs = currentTime[5], mlscs = currentTime[6];
    double UT = (hrs * 60 + mns) * 60 + scs + mlscs / 1000.0;
    
    long long d = dateToJDN(currentTime) - 2451545;
    double t = d / 36525.0;
    
    double S_0 = 
        (6 * 60 + 41)*60 + 50.54841 
        + 8640184.812866 * t + 0.093104 * t*t 
        - 6.2e-6 * t*t*t;

    double S = S_0 + UT;
    S *= Constants::Earth::ANGULAR_SPEED;

    return S;
}

Vector eci2ecef(double x, double y, double z, Vector currentTime)
{
    double S = greenwichSiderealTime(currentTime);

    // rotation matrix for greenwich and absolute geocentric convertion
    double a = cos(S), b = sin(S);
    double X, Y, Z = z;
    
    X = a*x + b*y;
    Y = a*y - b*x;
    
    return {X, Y, Z};
}

Vector eci2ecef(Vector eci, Vector currentTime)
{
    if (eci.size() == 3) {
        return eci2ecef(eci[0], eci[1], eci[2], currentTime);
    }
    else if (eci.size() == 6)
    {
        Vector ans(6);
        for (int part = 0; part < 2; part ++) {
            auto ecef = eci2ecef(eci[part*3], eci[part*3+1], eci[part*3+2], currentTime);
            for (int i = 0; i < 3; i ++)
                ans[part*3 + i] = ecef[i];
        }
        return ans;
    }
    return {};
}

Vector ecef2eci(double x, double y, double z, Vector currentTime)
{
    double S = greenwichSiderealTime(currentTime);

    // rotation matrix for greenwich and absolute geocentric convertion
    double a = cos(-S), b = sin(-S);
    double X, Y, Z = z;
    
    X = a*x + b*y;
    Y = a*y - b*x;
    
    return {X, Y, Z};
}

Vector ecef2eci(Vector ecef, Vector currentTime)
{
    if (ecef.size() == 3) {
        return ecef2eci(ecef[0], ecef[1], ecef[2], currentTime);
    }
    else if (ecef.size() == 6)
    {
        Vector ans(6);
        for (int part = 0; part < 2; part ++) {
            auto eci = ecef2eci(ecef[part*3], ecef[part*3+1], ecef[part*3+2], currentTime);
            for (int i = 0; i < 3; i ++)
                ans[part*3 + i] = eci[i];
        }
        return ans;
    }
    return {};
}

Vector pol2dec(const Vector &RLP)
{
    double r = RLP[0], l = RLP[1], p = RLP[2];
    double r1 = r * cos(p);
    
    return {
        r1 * cos(l),
        r1 * sin(l),
        r * sin(p)
    };
}

Vector dec2pol(const Vector &eci)
{
    double x = eci[0], y = eci[1], z = eci[2];
    double r1 = x*x+y*y;
    double r = sqrt(r1+z*z);
    
    double l = atan2(y,x);
    r1 = sqrt(r1);
    double p = atan2(z,r1);
    return {r, l, p};
}
