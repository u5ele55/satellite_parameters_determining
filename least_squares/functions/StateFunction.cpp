#include "StateFunction.hpp"

#include "integration/system/SpacecraftECI.hpp"
#include "integration/solver/RK4Solver.hpp"

#include "global/Constants.hpp"

#include "coordinates.hpp"
#include "time.hpp"

#include "radiotelescopes/TelescopeControl.hpp"

StateFunction::StateFunction(double time, TaskParameters *params)
:   time(time),
    params(params)
{}

Vector StateFunction::operator()(const Vector &x_0)
{
    RadioTelescope telescope(params->telescopeBLH, params->tsVisionAngle);
    TelescopeControl radioControl(telescope);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        x_0);
    RK4Solver solver(system, 10); 
    Vector state = solver.solve(time);
    
    return state;
}
