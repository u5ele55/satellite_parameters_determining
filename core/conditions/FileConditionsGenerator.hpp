#pragma once

#include "IConditionsGenerator.hpp"
#include <fstream>
#include <string>

class FileConditionsGenerator : public IConditionsGenerator
{
private:
    std::ifstream file;
    Vector MSEs;
public:
    FileConditionsGenerator(const std::string &filename, const Vector& MSEs);
    Conditions getConditions() override;
    ~FileConditionsGenerator();
};