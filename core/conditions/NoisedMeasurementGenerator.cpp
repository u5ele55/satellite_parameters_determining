#include "NoisedMeasurementGenerator.hpp"

#include "radiotelescopes/DesignationsNoiseApplier.hpp"
#include "integration/solver/RK4Solver.hpp"
#include "integration/system/SpacecraftECI.hpp"

#include "coordinates.hpp"
#include "time.hpp"

NoisedMeasurementGenerator::NoisedMeasurementGenerator(TaskParameters *params)
    : params(params)
{}

std::vector<Vector> NoisedMeasurementGenerator::generateMeasurements(const Vector &startState, std::vector<double> times)
{
    RadioTelescope telescope(params->telescopeBLH, params->tsVisionAngle);
    TelescopeControl radioControl(telescope);
    DesignationsNoiseApplier desNoiseApplier(radioControl, params->MSEs, 1);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        startState
    );
    RK4Solver solver(system, 1); 
    Vector currentTime(7);

    std::vector<Vector> measurements;

    for (double time : times) {
        Vector state = solver.solve(time);
        double x = state[3], y = state[4], z = state[5];
    
        Vector currentTime = JDToTime(params->JD + time / Constants::Earth::SECONDS_IN_DAY);
        Vector ecef = eci2ecef(x,y,z, currentTime);
        const auto& designation = desNoiseApplier.targetTelescope(ecef);
        measurements.push_back(designation);
    }

    return measurements;
}