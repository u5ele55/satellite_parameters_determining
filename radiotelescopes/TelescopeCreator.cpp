#include "TelescopeCreator.hpp"

#include <cmath>

TelescopeCreator::TelescopeCreator(const std::string &filename, bool inRadians)
    : file(filename), inRadians(inRadians)
{}

TelescopeCreator::~TelescopeCreator()
{
    file.close();
}

std::vector<RadioTelescope> TelescopeCreator::create()
{
    std::vector<RadioTelescope> res;

    int N;
    file >> N;
    for(int i = 0; i < N; i ++) {
        double b,l,h, angle;
        file >> b >> l >> h >> angle;
        if (!inRadians) {
            b *= M_PI / 180;
            l *= M_PI / 180;
            angle *= M_PI / 180;
        }
        res.emplace_back(Vector{b, l, h}, angle);
    }

    return res;
}
