#include "Core.hpp"
#include "LinAlg.hpp"
#include "Matrix.hpp"
#include "coordinates.hpp"
#include "time.hpp"

#include "global/Constants.hpp"
#include "global/TaskParameters.hpp"
#include "radiotelescopes/DesignationsNoiseApplier.hpp"
#include "integration/solver/RK4Solver.hpp"
#include "integration/system/SpacecraftECI.hpp"
#include "least_squares/ResidualsFunctionGenerator.hpp"
#include "least_squares/Iterator.hpp"

#include <iostream>
void Core::start()
{
    const double JD = 2460206.383;
    
    Vector telescopeBLH{30, 45, 1};

    LinAlg::toRad(telescopeBLH[0]);
    LinAlg::toRad(telescopeBLH[1]);
    Vector initialPosition = {6871257.864, 0.0, 0.0};
    Vector initialSpeed = {0.0, 3810.1125727278977, 6599.308558521686};
    double angleSecond = M_PI/(180 * 3600);
    auto parameters = new TaskParameters(
        telescopeBLH, 
        7 * M_PI / 180,
        JD,
        initialPosition[0], initialPosition[1], initialPosition[2],
        initialSpeed[0], initialSpeed[1], initialSpeed[2],
        100, 30 * angleSecond 
    );

    // generateMeasurements()
    generateMeasurements(*parameters);
    for(int i = 0; i < measurements.size(); i ++) {
        std::cout << times[i] << " " << measurements[i] << '\n';
    }
    // "worsen" initial state
    Vector initialGuess = {
        parameters->vx * 0.99,
        parameters->x + 800,
        parameters->vy * 1.01,
        parameters->y - 100,
        parameters->vz * 0.987,
        parameters->z + 200,
    };
    
    Iterator iterator(
        measurements, 
        times,
        initialGuess, 
        parameters
    );
    
    int iterations = 10;
    std::cout << "Starting with " << initialGuess << '\n';
    for (int j = 0; j < iterations; j ++) {
        std::cout << j << ": " << iterator.makeIteration() << '\n';
    }
    std::cout << "\nInit: " << parameters->initialState << '\n';
}

void Core::generateMeasurements(TaskParameters params)
{
    RadioTelescope telescope(params.telescopeBLH, params.tsVisionAngle);
    TelescopeControl radioControl(telescope);
    DesignationsNoiseApplier desNoiseApplier(radioControl, params.distMSE, params.angleMSE);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        params.initialState);
    RK4Solver solver(system, 10); 
    Vector currentTime(7);

    double step = 30;
    int hour = 3600;
    bool started = false;
    int cnt = 0;
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
        if (started) cnt ++;
        if (started) {
            // std::cout << "WAS " << ecef << '\n';
            measurements.push_back(designation);
            times.push_back(i);
        }
    }
}
