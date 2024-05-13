#pragma once
#include "Vector.hpp"
#include "global/Constants.hpp"

struct TaskParameters {
    TaskParameters(
        Vector telescopeBLH, 
        double tsVisionAngle,
        double JD,
        double x,
        double y,
        double z,
        double vx,
        double vy,
        double vz,
        Vector MSEs
    ) : TaskParameters(
            telescopeBLH, tsVisionAngle, JD, 
            {vx,vy,vz,x,y,z}, {}, MSEs
        )
        {};

    TaskParameters(
        Vector telescopeBLH, 
        double tsVisionAngle,
        double JD,
        Vector initialState,
        Vector guessState,
        Vector MSEs
    ) : telescopeBLH(telescopeBLH),
        tsVisionAngle(tsVisionAngle),
        JD(JD),
        initialState(initialState), MSEs(MSEs),
        guessState(guessState)
    {};

    Vector telescopeBLH;
    Vector initialState;
    Vector guessState;
    double tsVisionAngle;
    double JD;
    Vector MSEs;
};