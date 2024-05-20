#include "ConditionalPolynom.hpp"

#include <stdexcept>

ConditionalPolynom::ConditionalPolynom(
    std::vector<double> conditions, std::vector<int> degrees, std::vector<std::vector<double>> coefs
)
{
    if (conditions.size() + 1 != degrees.size() || conditions.size() + 1 != coefs.size()) {
        throw std::runtime_error("Size mismatch for conditional polynom");
    }
    for (int i = 0; i < degrees.size(); i ++) {
        use.emplace_back(conditions[i], new Polynom(degrees[i], coefs[i]));
    }
}

double ConditionalPolynom::operator()(double x)
{
    int index = 0;
    for (int i = 0; i < use.size(); i ++) {
        index = i;
        if (x <= use[i].first) {
            break;
        }
    }
    auto polynom = *use[index].second;
    return polynom(x);
}
