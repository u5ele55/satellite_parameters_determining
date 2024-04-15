#include "ResidualsFunctionGenerator.hpp"

#include "ResidualsFunction.hpp"

ResidualsFunctionGenerator::ResidualsFunctionGenerator(
    std::vector<Vector> measurements, std::vector<double> times, TaskParameters *params
)   : measurements(measurements),
    times(times),
    params(params)
{}

std::vector<Function<const Vector &, Vector>*> ResidualsFunctionGenerator::generate()
{
    std::vector<Function<const Vector &, Vector>*> res;
    for(int i = 0; i < measurements.size(); i ++) {
        res.push_back(new ResidualsFunction(measurements[i], times[i], params));
    }
    return res;
}
