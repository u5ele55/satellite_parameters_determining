#pragma once
#include "IMeasurementGenerator.hpp"
#include "global/TaskParameters.hpp"

class TrueMeasurementGenerator : public IMeasurementGenerator
{
private:
    TaskParameters *params;
public:
    TrueMeasurementGenerator(TaskParameters *params);
    std::vector<Vector> generateMeasurements(const Vector &startState, std::vector<double> times) override;
};