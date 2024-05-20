#pragma once

#include "AbstractFileOutputter.hpp"
#include <vector>

#include <memory> // for std::allocator
template <typename what>
class FileOutputter : public AbstractFileOutputter<what>
{
public:
    FileOutputter(const std::string& filename) : AbstractFileOutputter<what>(filename) {};
    void output(std::vector<what> data) override;
};

template <typename what>
inline void FileOutputter<what>::output(std::vector<what> data)
{
    for (const auto &e : data)
        this->file << e << '\n';
}
