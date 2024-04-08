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
#include "least_squares/DesignationFunctionGenerator.hpp"
#include "least_squares/Iterator.hpp"

#include "output/FileOutputter.hpp"

#include <iostream>
void Core::start()
{
    Vector currentTime = {2023, 9, 18, 21, 11, 31, 0};
    double JD = timeToJD(currentTime);
    
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

    FileOutputter<Vector> outputMeasurements("measurements.txt");
    FileOutputter<Vector> outputDesGuess("guess_measurements.txt");
    FileOutputter<Vector> outputDesResult("result_measurements.txt");
    FileOutputter<double> outputTime("time.txt");
    
    // generateMeasurements()
    generateMeasurements(*parameters);
    outputMeasurements.output(measurements);
    outputTime.output(times);
    
    // "worsen" initial state
    Vector initialGuess = {
        parameters->vx * 0.99,
        parameters->vy * 1.01,
        parameters->vz * 0.987,
        parameters->x + 800,
        parameters->y - 100,
        parameters->z + 100,
    };

    DesignationFunctionGenerator desGen(times, parameters);
    auto des = desGen.generate();
    std::vector<Vector> guessDes;
    for (auto *d : des) {
        guessDes.push_back( (*d)(initialGuess) );
    }
    outputDesGuess.output(guessDes);
    
    Iterator iterator(
        measurements, 
        times,
        initialGuess, 
        parameters
    );
    
    int iterations = 3;
    std::cout << "Starting with " << initialGuess << '\n';
    Vector q(6);
    for (int j = 0; j < iterations; j ++) {
        q = iterator.makeIteration();
        std::cout << j << ": " << q << '\n';
    }
    std::cout << "\nInit: " << parameters->initialState << '\n';

    des = desGen.generate();
    std::vector<Vector> newDes;
    for (auto *d : des) {
        newDes.push_back( (*d)(q) );
    }
    outputDesResult.output(newDes);
}

void Core::generateMeasurements(TaskParameters params)
{
    RadioTelescope telescope(params.telescopeBLH, params.tsVisionAngle);
    TelescopeControl radioControl(telescope);
    DesignationsNoiseApplier desNoiseApplier(radioControl, params.distMSE, params.angleMSE, 1);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        params.initialState);
    RK4Solver solver(system, 10); 
    Vector currentTime(7);

    double step = 10;
    int hour = 3600;
    bool started = false;
    int cnt = 0;
    for (int i = 0; i <= 10 * hour; i += step) {
        double time = i;
        Vector state = solver.solve(time);
        double x = state[3], y = state[4], z = state[5];
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
