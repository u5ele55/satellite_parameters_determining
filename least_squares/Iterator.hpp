#pragma once

#include "Vector.hpp"
#include "global/TaskParameters.hpp"
#include "Function/Function.hpp"

// #include "output/FileOutputter.hpp"

class Iterator
{
private:
    Vector initialGuess;
    Vector q;
    std::vector<Vector> measurements;
    std::vector<double> times;
    // Vector MSEs;
    std::vector<Function<const Vector&, Vector>*> desFunctions;
    TaskParameters *params;
    Vector diagonalK;
    // FileOutputter<Vector> output;
public:
    Iterator(
        std::vector<Vector> measurements, 
        std::vector<double> times, 
        Vector initialGuess, 
        TaskParameters *params
    );
    ~Iterator();
    Vector makeIteration();
    
};