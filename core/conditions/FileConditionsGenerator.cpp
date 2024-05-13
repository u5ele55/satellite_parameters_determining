#include "FileConditionsGenerator.hpp"
#include "LinAlg.hpp"
#define READ_TRASH(in, a) for(int i=0; i < a; i ++) in >> trash;

FileConditionsGenerator::FileConditionsGenerator(const std::string &filename)
    : file(filename)
{}
#include <iostream>
Conditions FileConditionsGenerator::getConditions()
{
    Vector BLH(3);
    Vector initialState(6);
    Vector guessState(6);
    double tsVisionAngle = 7;
    double angleSecond = M_PI/(180 * 3600);
    Vector MSEs = {1,1,1};//{100, 7*angleSecond, 7*angleSecond};
    double JD;
    std::string trash;

    READ_TRASH(file, 4);
    file >> BLH[0] >> trash >> trash 
         >> BLH[1] >> trash >> trash 
         >> BLH[2] >> trash >> trash;
    file >> JD;
    JD += 2400000.5; // MJD -> JD
    READ_TRASH(file, 8);
    // std::cout << BLH[0] << " " << BLH[1] << '\n';

    for (int i = 3; i < 6; i ++)
        file >> initialState[i];
    for (int i = 0; i < 3; i ++)
        file >> initialState[i];
    READ_TRASH(file, 4);
    for (int i = 3; i < 6; i ++)
        file >> guessState[i];
    for (int i = 0; i < 3; i ++)
        file >> guessState[i];
    initialState *= 1000;
    guessState *= 1000;
    // std::cout << initialState << ' ' << guessState << '\n';
    LinAlg::toRad(BLH[0]);
    LinAlg::toRad(BLH[1]);
    LinAlg::toRad(tsVisionAngle);
    Conditions conditions;
    conditions.parameters = new TaskParameters(
        BLH, tsVisionAngle, JD, initialState, guessState, MSEs
    );
    int measCnt = 0;
    Vector measurement(3);
    double time;

    while (!file.eof()) {
        file >> time >> measurement[0] >> measurement[1] >> measurement[2] >> trash >> trash;
        measurement[0] *= 1000;
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
