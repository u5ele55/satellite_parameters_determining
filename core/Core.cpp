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
#include "integration/system/SpacecraftECEF.hpp"
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

    auto *params = conditions.parameters;

    FileOutputter<Vector> outputMeasurements("measurements.txt");
    FileOutputter<Vector> outputDesGuess("guess_measurements.txt");
    FileOutputter<Vector> outputDesResult("result_measurements.txt");
    FileOutputter<double> outputTime("time.txt");
    
    // generateMeasurements()
    outputTime.output(conditions.times);
    // return;
    params->guessState += Vector{150, -90, 250, -10000, 10000, -9000} / 1000; // just for tests, adding even more noise

    IMeasurementGenerator *measGenerator;
    if ((params->MSEs - Vector{1,1,1}).norm() < 1e-9) {
        measGenerator = new TrueMeasurementGenerator(conditions.parameters);
    } else {
        measGenerator = new NoisedMeasurementGenerator(conditions.parameters);
    }
    conditions.measurements = measGenerator->generateMeasurements(
        params->initialStateECEF, conditions.times
    );
    outputMeasurements.output(conditions.measurements);

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
    auto deltaQ = params->guessState - params->initialStateECEF;
    Vector dV = deltaQ.subvector(0, 2);
    Vector dR = deltaQ.subvector(3, 5);
    
    std::cout << "  |dR| = " << dR.norm() << "  |dV| = " << dV.norm() << "\n";
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
            auto deltaQ = q - params->initialStateECEF;
            Vector dV = deltaQ.subvector(0, 2);
            Vector dR = deltaQ.subvector(3, 5);
            
            std::cout << "  |dR| = " << dR.norm() << "  |dV| = " << dV.norm() << "\n";
        }
    } while (!shouldStop(q, lastQ));

    std::cout << "\nFinal: " << q << '\n';

    std::cout << "\nTrue: " << params->initialStateECEF << '\n';
    std::cout << "RSS of true: " << calcRSS(params->initialStateECEF) << '\n';
    
    std::vector<Vector> newDes = measGenerator->generateMeasurements(q, conditions.times);
    outputDesResult.output(newDes);

    // restoring satellite state on the moment of intersecting equator
    std::cout << "\nSatellite intersecting equator\n";
    Vector inverted = q;
    auto *system = new SpacecraftECEF(
        Constants::Satellite::Sb,
        inverted
    );
    RK4Solver solver(system, 1);

    double a = q.subvector(3,5).norm() / 2; // approx. semi-major axis of an orbit
    double T = 2 * M_PI * sqrt( pow(a, 3) / (Constants::Common::G * Constants::Earth::MASS) ); // period
    double l = -T/2, r = 0;

    auto zeta = [](Vector xyz) {
        auto polar = dec2pol(xyz);
        return polar[2] < 0; // latitude
    };
    bool isUnderEquator = zeta(q.subvector(3,5));

    auto check = [&isUnderEquator, &solver, &zeta](double time) {
        auto state = solver.solve(time);
        return isUnderEquator ^ zeta(state.subvector(3,5));
    };

    std::cout << "  Binary search from " << l << " to " << r << "; initially under equator: " << isUnderEquator << '\n';
    while (r - l > 0.001) {
        double midtime = (r+l)/2;
        // std::cout << midtime << " " << check(midtime) << '\n';
        if (check(midtime)) {
            l = midtime;
        } else {
            r = midtime;
        }
    }
    auto equatorTime = JDToTime(params->JD - l / Constants::Earth::SECONDS_IN_DAY);
    
    std::cout << "  Intersecting equator at t = " << l << " = " << equatorTime << '\n';
    Vector equatorPoint = solver.solve(l);
    std::cout << "  Equator point ECEF: " << equatorPoint << '\n';
    std::cout << "  Equator point ECI: " << ecef2eci(equatorPoint.subvector(3,5), equatorTime) << '\n';
}
