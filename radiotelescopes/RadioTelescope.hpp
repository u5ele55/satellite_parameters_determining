#pragma once

#include "../utils/Vector.hpp"

class RadioTelescope {
public:
    RadioTelescope(const Vector &blh, double angle) : blh(blh), angle(angle) {};
    Vector getBLH() const;
    double getAngle() const;
private:
    Vector blh;
    double angle;
}; 