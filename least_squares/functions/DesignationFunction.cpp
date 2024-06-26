#include "DesignationFunction.hpp"

#include "integration/system/SpacecraftECEF.hpp"
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

    auto *system = new SpacecraftECEF(
        Constants::Satellite::Sb,
        arg
    );
    RK4Solver solver(system, 10); 
    Vector state = solver.solve(time);
    double x = state[3], y = state[4], z = state[5];
    
    Vector currentTime = JDToTime(params->JD + time / Constants::Earth::SECONDS_IN_DAY);
    // Vector ecef = eci2ecef(x,y,z, currentTime);
    auto des = radioControl.targetTelescope({x,y,z});
    // std::cout << arg << " -> " << ecef << " -> " << des <<'\n';

    return des;
}
