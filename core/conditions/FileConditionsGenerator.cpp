#include "FileConditionsGenerator.hpp"
#include "LinAlg.hpp"
#include "coordinates.hpp"
#include "time.hpp"
#define READ_TRASH(in, a) for(int i=0; i < a; i ++) in >> trash;

FileConditionsGenerator::FileConditionsGenerator(const std::string &filename, const Vector& MSEs)
    : file(filename),
    MSEs(MSEs)
{}
#include <iostream>
Conditions FileConditionsGenerator::getConditions()
{
    Vector BLH(3);
    Vector guessState(6);
    double tsVisionAngle = 7;
    double JD;
    std::string trash;

    READ_TRASH(file, 4);
    file >> BLH[0] >> trash >> trash 
         >> BLH[1] >> trash >> trash 
         >> BLH[2] >> trash >> trash;
    file >> JD;
    JD += 2400000.5; // MJD -> JD
    Vector currentTime = JDToTime(JD);
    
    READ_TRASH(file, 8);
    // std::cout << BLH[0] << " " << BLH[1] << '\n';
    double x,y,z, vx,vy,vz;
    file >> x >> y >> z >> vx >> vy >> vz;
    Vector initialStateECEF = {vx,vy,vz,x,y,z};
    Vector initialStateECI = ecef2eci(initialStateECEF, currentTime);

    READ_TRASH(file, 4);
    file >> x >> y >> z >> vx >> vy >> vz;
    guessState = {vx,vy,vz,x,y,z};
    
    LinAlg::toRad(BLH[0]);
    LinAlg::toRad(BLH[1]);
    LinAlg::toRad(tsVisionAngle);
    Conditions conditions;
    conditions.parameters = new TaskParameters(
        BLH, tsVisionAngle, JD, initialStateECI, initialStateECEF, guessState, MSEs
    );
    int measCnt = 0;
    Vector measurement(3);
    double time;

    while (!file.eof()) {
        file >> time >> measurement[0] >> measurement[1] >> measurement[2] >> trash >> trash;
        // measurement[0] *= 1000;
        LinAlg::toRad(measurement[1]); LinAlg::toRad(measurement[2]);
        conditions.measurements.push_back(measurement);
        conditions.times.push_back(time);
    }
    return conditions;
}

FileConditionsGenerator::~FileConditionsGenerator()
{
    file.close();
}
