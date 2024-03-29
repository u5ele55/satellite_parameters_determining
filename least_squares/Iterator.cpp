#include "Iterator.hpp"

Iterator::Iterator(std::vector<Vector> measurements, Vector initialGuess, Vector MSEs)
    : initialGuess(initialGuess),
    MSEs(MSEs),
    measurements(measurements),
    q(initialGuess)
{}

Vector Iterator::makeIteration()
{
    
}
