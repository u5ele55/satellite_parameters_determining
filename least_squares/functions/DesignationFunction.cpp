#include "DesignationFunction.hpp"

#include "integration/system/SpacecraftECI.hpp"
#include "integration/solver/RK4Solver.hpp"

#include "global/Constants.hpp"

#include "coordinates.hpp"
#include "time.hpp"

#include "radiotelescopes/TelescopeControl.hpp"

DesignationFunction::DesignationFunction(double time, TaskParameters *params)
    : time(time),
    params(params)
{}
#include <iostream>
Vector DesignationFunction::operator()(const Vector &arg)
{
    RadioTelescope telescope(params->telescopeBLH, params->tsVisionAngle);
    TelescopeControl radioControl(telescope);

    double x = arg[3], y = arg[4], z = arg[5];
    long long t = time + params->unixTimestamp;

    Vector currentTime = unixToTime(t);
    Vector ecef = eci2ecef(x,y,z, currentTime);
    auto des = radioControl.targetTelescope(ecef);

    return des;
}
