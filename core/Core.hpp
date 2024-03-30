#pragma once

#include "Vector.hpp"

class Core
{
private:
    std::vector<Vector> measurements;
    std::vector<double> times;

    
public:
    void start();
private:   
    void generateMeasurements(TaskParameters params);
};
