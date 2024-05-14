#pragma once

#include "Vector.hpp"

class IMeasurementGenerator
{
public:
    virtual std::vector<Vector> generateMeasurements(const Vector &startState, std::vector<double> times) = 0;
    ~IMeasurementGenerator() {};
};