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
#include "TestFunction.hpp"

#include <iostream>
void Core::start()
{
    Vector currentTime = {2023, 9, 18, 21, 11, 31, 0};
    double JD = timeToJD(currentTime);
    
    Vector telescopeBLH{45, 45, 1};

    LinAlg::toRad(telescopeBLH[0]);
    LinAlg::toRad(telescopeBLH[1]);

    FileOutputter<Vector> outputMeasurements("measurements.txt");
    FileOutputter<Vector> outputDesGuess("guess_measurements.txt");
    FileOutputter<Vector> outputDesResult("result_measurements.txt");
    FileOutputter<double> outputTime("time.txt");
    
    // generateMeasurements()
    generateMeasurements(TaskParameters{0,0,0,0,0,0,0,0,0,0,0});
    outputMeasurements.output(measurements);
    outputTime.output(times);
    
    // "worsen" initial state
    Vector initialGuess = {
        0.5, 2, 2.5, 4, 4.5, 6
    };
    
    Iterator iterator(
        measurements, 
        times,
        initialGuess, 
        nullptr
    );
    
    int iterations = 1;
    std::cout << "Starting with " << initialGuess << '\n';
    Vector q(6);
    for (int j = 0; j < iterations; j ++) {
        q = iterator.makeIteration();
        std::cout << j << ": " << q << '\n';
    }
}

void Core::generateMeasurements(TaskParameters params)
{
    for (int i = 10; i <= 90; i += 10) {
        TestFunction tf(i);
        auto designation = tf({1, 2, 3, 4, 5, 6});
        
        // std::cout << designation << '\n';
        measurements.push_back(designation);
        times.push_back(i);
    }
}
