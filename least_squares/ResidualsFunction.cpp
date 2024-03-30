#include "ResidualsFunction.hpp"

#include "integration/system/SpacecraftECI.hpp"
#include "integration/solver/RK4Solver.hpp"

#include "global/Constants.hpp"

#include "coordinates.hpp"
#include "time.hpp"

#include "radiotelescopes/TelescopeControl.hpp"

ResidualsFunction::ResidualsFunction(
    std::vector<Vector> measurements, 
    std::vector<double> times,
    TaskParameters params
) : measurements(measurements),
    times(times),
    params(params)
{}

Vector ResidualsFunction::operator()(const Vector& x)
{
    RadioTelescope telescope(params.telescopeBLH, params.tsVisionAngle);
    TelescopeControl radioControl(telescope);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        x);
    RK4Solver solver(system, 10); 
    Vector currentTime(7);

    double step = 30;
    int hour = 3600;
    bool started = false;
    for (int i = 0; i < times.size(); i ++) {
        double time = times[i];
        Vector state = solver.solve(time);
        double x = state[1], y = state[3], z = state[5];
        long long t = time + params.unixTimestamp;

        currentTime = unixToTime(t);
        Vector ecef = eci2ecef(x,y,z, currentTime);

        const Vector& designation = radioControl.targetTelescope(ecef);
        Vector r = designation - measurements[i];
    }
}
