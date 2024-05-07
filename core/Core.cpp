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
#include "least_squares/functions/DesignationFunctionGenerator.hpp"
#include "least_squares/Iterator.hpp"
#include "least_squares/functions/ResidualsFunctionGenerator.hpp"

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
    Vector initialSpeed = {0.0, 3810, 6600};
    double angleSecond = M_PI/(180 * 3600);

    Vector trueK = {1, 1, 1};
    Vector noisedK = {100, 7*angleSecond, 7*angleSecond};

    auto parameters = new TaskParameters(
        telescopeBLH, 
        7 * M_PI / 180,
        JD,
        initialPosition[0], initialPosition[1], initialPosition[2],
        initialSpeed[0], initialSpeed[1], initialSpeed[2],
        noisedK
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
        parameters->vx + 50,
        parameters->vy + 50,
        parameters->vz - 55,
        parameters->x + 5000,
        parameters->y - 5200,
        parameters->z + 5000,
    };

    // initial guess measurements
    DesignationFunctionGenerator desGen(times, parameters);
    auto des = desGen.generate();
    std::vector<Vector> guessDes;
    for (auto *d : des) {
        guessDes.push_back( (*d)(initialGuess) );
    }
    outputDesGuess.output(guessDes);
    // return;
    // iterating setup
    Iterator iterator(
        measurements, 
        times,
        initialGuess, 
        parameters
    );
    
    std::cout << "Starting with " << initialGuess << '\n';
    Vector q(6), lastQ(6);

    auto shouldStop = [](const Vector &q, const Vector &lastQ) {
        Vector delta = lastQ-q;
        Vector v = {delta[0], delta[1], delta[2]};
        Vector r = {delta[3], delta[4], delta[5]};
        return v.norm() < 1e-3 && r.norm() < 1e-3;
    };
    ResidualsFunctionGenerator resGen(measurements, times, parameters);
    auto resFs = resGen.generate();
    
    auto calcResSq = [&resFs, &parameters](Vector st) {
        double r = 0;
        for (auto& res : resFs) {
            auto resV = (*res)(st);
            for (int i = 0; i < resV.size(); i ++)
                resV[i] /= pow(parameters->MSEs[i], 2);
            r += resV.dot(resV);
        }
        return r;
    };

    // iterating process
    int iter = 0;
    do {
        lastQ = q;
        iter ++;
        std::cout << "\nIteration No. " << iter << '\n';
        q = iterator.makeIteration();
        if (iter != 0)  {
            std::cout << "  Q: " << q << '\n';
            std::cout << "  RSS: " << calcResSq(q) << '\n';
            auto deltaQ = q - parameters->initialState;
            Vector dV = deltaQ.subvector(0, 2);
            Vector dR = deltaQ.subvector(3, 5);
            
            std::cout << "  |dR| = " << dR.norm() << "  |dV| = " << dV.norm() << "\n";
        }
    } while (!shouldStop(q, lastQ));

    std::cout << "\nFinal: " << q << '\n';

    std::cout << "\nInit: " << parameters->initialState << '\n';
    std::cout << "RSS of init: " << calcResSq(parameters->initialState) << '\n';
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
    DesignationsNoiseApplier desNoiseApplier(radioControl, params.MSEs, 1);

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        params.initialState
    );
    RK4Solver solver(system, 10); 
    Vector currentTime(7);

    double step = 10;
    int minute = 60;
    int hour = 60 * minute;
    
    for (int i = 150; i <= 22 * minute; i += step) {
        double time = i;
        Vector state = solver.solve(time);
        double x = state[3], y = state[4], z = state[5];
        long long t = i + params.unixTimestamp;
        
        currentTime = unixToTime(t);
        Vector ecef = eci2ecef(x,y,z, currentTime);

        const auto& designation = desNoiseApplier.targetTelescope(ecef);
 
        measurements.push_back(designation);
        times.push_back(i);
    }
    
}
