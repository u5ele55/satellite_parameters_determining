#pragma once

#include "IConditionsGenerator.hpp"
#include <fstream>
#include <string>

class FileConditionsGenerator : public IConditionsGenerator
{
private:
    std::ifstream file;
public:
    FileConditionsGenerator(const std::string &filename);
    Conditions getConditions() override;
    ~FileConditionsGenerator();
};