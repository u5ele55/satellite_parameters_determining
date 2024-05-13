#pragma once
#include "Vector.hpp"

Vector blh2ecef(const Vector &blh);
Vector ecef2blh(const Vector &ecef);
Vector eci2ecef(double x, double y, double z, Vector currentTime);

/// @brief Перевод полярных координат в декартовы
/// @param RLP Радиус, долгота, широта
Vector pol2dec(const Vector &RLP);