#pragma once
#include "Vector.hpp"
#include "global/Constants.hpp"

struct TaskParameters {
    TaskParameters(
        Vector telescopeBLH, 
        double tsVisionAngle,
        double JD,
        double x,
        double y,
        double z,
        double vx,
        double vy,
        double vz,
        double distMSE,
        double angleMSE
    ) : telescopeBLH(telescopeBLH),
        tsVisionAngle(tsVisionAngle),
        JD(JD),
        unixTimestamp((JD - 2440587.5) * Constants::Earth::SECONDS_IN_DAY),
        x(x), y(y), z(z), vx(vx), vy(vy), vz(vz), initialState(6),
        distMSE(distMSE), angleMSE(angleMSE)
    {
        initialState = {vx, x, vy, y, vz, z};
    };

    Vector telescopeBLH;
    Vector initialState;
    double tsVisionAngle;
    double JD;
    double unixTimestamp;
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;
    double distMSE;
    double angleMSE;
};