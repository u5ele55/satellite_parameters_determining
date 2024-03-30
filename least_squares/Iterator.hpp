#pragma once

#include "Vector.hpp"

class Iterator
{
private:
    Vector initialGuess;
    Vector q;
    std::vector<Vector> measurements;
    Vector MSEs;
public:
    Iterator(std::vector<Vector> measurements, Vector initialGuess, Vector MSEs);
    Vector makeIteration();
    
};