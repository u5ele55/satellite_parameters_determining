#pragma once

#include "TestFunction.hpp"
#include <vector>

class TestFunctionGenerator
{
private:
    std::vector<double> times;
public:
    TestFunctionGenerator(std::vector<double> times) : times(times) {};
    std::vector<Function<const Vector&, Vector>*> generate() {
        std::vector<Function<const Vector&, Vector>*> ans;
        for (auto t : times) {
            ans.push_back(new TestFunction(t));
        }
        return ans;
    };
};
