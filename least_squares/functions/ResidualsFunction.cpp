#include "ResidualsFunction.hpp"

#include "integration/system/SpacecraftECEF.hpp"
#include "integration/solver/RK4Solver.hpp"

#include "global/Constants.hpp"

#include "coordinates.hpp"
#include "time.hpp"

#include "radiotelescopes/TelescopeControl.hpp"

ResidualsFunction::ResidualsFunction(
    Vector measurement, double time, TaskParameters *params
) : measurement(measurement),
    time(time),
    params(params)
{}
#include <iostream>
Vector ResidualsFunction::operator()(const Vector &arg)
{
    RadioTelescope telescope(params->telescopeBLH, params->tsVisionAngle);
    TelescopeControl radioControl(telescope);

    auto *system = new SpacecraftECEF(
        Constants::Satellite::Sb,
        arg
    );
    RK4Solver solver(system, 10); 
    Vector state = solver.solve(time);
    double x = state[3], y = state[4], z = state[5];

    Vector currentTime = JDToTime(time + params->JD / Constants::Earth::SECONDS_IN_DAY);
    Vector ecef = eci2ecef(x,y,z, currentTime);

    const Vector& designation = radioControl.targetTelescope(ecef);
    if (!designation.size()) {
        return {};
    }
    Vector r = designation - measurement;

    return r;
}
