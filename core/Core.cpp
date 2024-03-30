#include "Core.hpp"
#include "radiotelescopes/DesignationsNoiseApplier.hpp"
#include "LinAlg.hpp"
#include "Matrix.hpp"
#include "coordinates.hpp"
#include "time.hpp"
#include "global/Constants.hpp"
#include "integration/solver/RK4Solver.hpp"
#include "integration/system/SpacecraftECI.hpp"

#include "global/TaskParameters.hpp"

#include <iostream>
void Core::start()
{
    const double JD = 2460206.383;
    
    Vector telescopeBLH{30, 45, 1};

    LinAlg::toRad(telescopeBLH[0]);
    LinAlg::toRad(telescopeBLH[1]);
    Vector initialPosition = {6871257.864, 0.0, 0.0};
    Vector initialSpeed = {0.0, 3810.1125727278977, 6599.308558521686};

    auto parameters = TaskParameters(
        telescopeBLH, 
        7 * M_PI / 180,
        JD,
        initialPosition[0], initialPosition[1], initialPosition[2],
        initialSpeed[0], initialSpeed[1], initialSpeed[2] 
    );

    // generateMeasurements()
    generateMeasurements(parameters);
    for(int i = 0; i < measurements.size(); i ++) {
        std::cout << times[i] << " " << measurements[i] << '\n';
    }
    // applyNoiseToState()
    
    // try initial approximation 
    // .calculateResiduals()
    // make step of gradient descent / newton method
    // .step()
    // if tol < eps end
}

void Core::generateMeasurements(TaskParameters params)
{
    double angleSecond = M_PI/(180 * 3600);
    RadioTelescope telescope(params.telescopeBLH, params.tsVisionAngle);
    TelescopeControl radioControl(telescope);
    DesignationsNoiseApplier desNoiseApplier(radioControl, 100, 30 * angleSecond);

    Vector initialState{
        params.vx, params.x,
        params.vy, params.y,
        params.vz, params.z
        };
    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        initialState);
    RK4Solver solver(system, 10); 
    Vector currentTime(7);

    double step = 30;
    int hour = 3600;
    bool started = false;
    for (int i = 0; i <= 10 * hour; i += step) {
        double time = i;
        Vector state = solver.solve(time);
        double x = state[1], y = state[3], z = state[5];
        long long t = i + params.unixTimestamp;

        currentTime = unixToTime(t);
        Vector ecef = eci2ecef(x,y,z, currentTime);

        const auto& designation = desNoiseApplier.targetTelescope(ecef);
        if (!started && designation.size() == 3) {
            started = true;
        }
        if (started && designation.size() < 3) {
            break;
        }
        if (started) {
            measurements.push_back(designation);
            times.push_back(i);
        }
    }
}
