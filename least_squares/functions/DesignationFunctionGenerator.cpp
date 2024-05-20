#include "DesignationFunctionGenerator.hpp"

#include "DesignationFunction.hpp"

DesignationFunctionGenerator::DesignationFunctionGenerator(
    std::vector<double> times, TaskParameters *params
)   : times(times),
    params(params)
{}

std::vector<Function<const Vector &, Vector>*> DesignationFunctionGenerator::generate()
{
    std::vector<Function<const Vector &, Vector>*> res;
    for(int i = 0; i < times.size(); i ++) {
        res.push_back(new DesignationFunction(times[i], params));
    }
    return res;
}
