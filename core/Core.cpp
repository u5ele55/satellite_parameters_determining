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

#include "conditions/FileConditionsGenerator.hpp"
#include "conditions/NoisedMeasurementGenerator.hpp"
#include "conditions/TrueMeasurementGenerator.hpp"

#include <iostream>

void Core::start()
{
    double angleSecond = M_PI/(180 * 3600);
    Vector trueMSEs = {1,1,1}; 
    Vector noiseMSEs = {100e-6, 7*angleSecond, 7*angleSecond};

    Vector MSEs = noiseMSEs;
    
    IConditionsGenerator *condGen = new FileConditionsGenerator("../datafiles/sample1.txt", MSEs);
    
    conditions = condGen->getConditions();

    FileOutputter<Vector> outputMeasurements("measurements.txt");
    FileOutputter<Vector> outputDesGuess("guess_measurements.txt");
    FileOutputter<Vector> outputDesResult("result_measurements.txt");
    FileOutputter<double> outputTime("time.txt");
    
    // generateMeasurements()
    outputMeasurements.output(conditions.measurements);
    outputTime.output(conditions.times);

    auto *params = conditions.parameters;
    params->guessState += {150, -90, 250, -10000, 10000, -9000}; // just for tests, adding even more noise
    // initial guess measurements
    DesignationFunctionGenerator desGen(conditions.times, params);
    auto des = desGen.generate();

    IMeasurementGenerator *measGenerator;
    if ((params->MSEs - Vector{1,1,1}).norm() < 1e-9) {
        measGenerator = new TrueMeasurementGenerator(conditions.parameters);
    } else {
        measGenerator = new NoisedMeasurementGenerator(conditions.parameters);
    }
    conditions.measurements = measGenerator->generateMeasurements(
        params->initialState, conditions.times
    );

    std::vector<Vector> guessDes = measGenerator->generateMeasurements(params->guessState, conditions.times);
    outputDesGuess.output(guessDes);

    std::cout << "Iteration process\n";
    // iterating setup
    Iterator iterator(
        conditions.measurements, 
        conditions.times,
        params->guessState, 
        params
    );
    
    std::cout << "Starting with " << params->guessState << '\n';
    Vector q(6), lastQ(6);

    auto shouldStop = [](const Vector &q, const Vector &lastQ) {
        Vector delta = lastQ-q;
        Vector v = delta.subvector(0, 2); // {delta[0], delta[1], delta[2]};
        Vector r = delta.subvector(3, 5); // {delta[3], delta[4], delta[5]};
        return v.norm() < 1e-3 && r.norm() < 1e-3;
    };
    ResidualsFunctionGenerator resGen(conditions.measurements, conditions.times, params);
    auto resFs = resGen.generate();
    
    auto calcRSS = [&resFs, &params](Vector st) {
        double r = 0;
        for (auto& res : resFs) {
            auto resV = (*res)(st);
            for (int i = 0; i < resV.size(); i ++)
                resV[i] /= pow(params->MSEs[i], 2);
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
            std::cout << "  RSS: " << calcRSS(q) << '\n';
            auto deltaQ = q - params->initialState;
            Vector dV = deltaQ.subvector(0, 2);
            Vector dR = deltaQ.subvector(3, 5);
            
            std::cout << "  |dR| = " << dR.norm() << "  |dV| = " << dV.norm() << "\n";
        }
    } while (!shouldStop(q, lastQ));

    std::cout << "\nFinal: " << q << '\n';

    std::cout << "\nInit: " << params->initialState << '\n';
    std::cout << "RSS of init: " << calcRSS(params->initialState) << '\n';
    
    std::vector<Vector> newDes = measGenerator->generateMeasurements(q, conditions.times);
    outputDesResult.output(newDes);
}
