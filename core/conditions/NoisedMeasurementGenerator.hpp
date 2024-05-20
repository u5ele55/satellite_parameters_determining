#pragma once
#include "IMeasurementGenerator.hpp"
#include "global/TaskParameters.hpp"

class NoisedMeasurementGenerator : public IMeasurementGenerator
{
private:
    TaskParameters *params;
public:
    NoisedMeasurementGenerator(TaskParameters *params);
    std::vector<Vector> generateMeasurements(const Vector &startState, std::vector<double> times) override;
};