#pragma once

#include "Conditions.hpp"

class IConditionsGenerator
{
public:
    virtual Conditions getConditions() = 0;
    ~IConditionsGenerator() {};
};
