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

#include <iostream>

void Core::start()
{
    IConditionsGenerator *condGen = new FileConditionsGenerator("../datafiles/sample1.txt");
    
    conditions = condGen->getConditions();

    FileOutputter<Vector> outputMeasurements("measurements.txt");
    FileOutputter<Vector> outputDesGuess("guess_measurements.txt");
    FileOutputter<Vector> outputDesResult("result_measurements.txt");
    FileOutputter<double> outputTime("time.txt");
    
    // generateMeasurements()
    outputMeasurements.output(conditions.measurements);
    outputTime.output(conditions.times);

    auto *params = conditions.parameters;
    // initial guess measurements
    std::cout << params->unixTimestamp << '\n';
    std::cout << "guessDes\n";
    DesignationFunctionGenerator desGen(conditions.times, params);
    auto des = desGen.generate();
    std::vector<Vector> guessDes;
    for (auto *d : des) {
        guessDes.push_back( (*d)(params->guessState) );
    }
    outputDesGuess.output(guessDes);
    std::cout << "go iter\n";
    // return;
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
    des = desGen.generate();
    std::vector<Vector> newDes;
    for (auto *d : des) {
        newDes.push_back( (*d)(q) );
    }
    outputDesResult.output(newDes);
}
