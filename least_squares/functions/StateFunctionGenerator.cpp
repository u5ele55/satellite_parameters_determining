#include "StateFunctionGenerator.hpp"

#include "StateFunction.hpp"

StateFunctionGenerator::StateFunctionGenerator(
    std::vector<double> times, TaskParameters *params
)   : times(times),
    params(params)
{}

std::vector<Function<const Vector &, Vector>*> StateFunctionGenerator::generate()
{
    std::vector<Function<const Vector &, Vector>*> res;
    for(int i = 0; i < times.size(); i ++) {
        res.push_back(new StateFunction(times[i], params));
    }
    return res;
}
