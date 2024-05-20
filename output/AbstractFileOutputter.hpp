#pragma once

#include "IOutputter.hpp"
#include <fstream>

template <typename T>
class AbstractFileOutputter : public IOutputter<T>
{
protected:
    std::ofstream file;
public:
    AbstractFileOutputter(const std::string &filename) : file(filename) {};
    virtual ~AbstractFileOutputter() { file.close(); };
};
