#pragma once

#include <vector>

template <typename T>
class IOutputter
{
public:
    virtual void output(std::vector<T> data) = 0;
    virtual ~IOutputter() {};
};