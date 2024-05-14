#pragma once
#include "Vector.hpp"

Vector blh2ecef(const Vector &blh);
Vector ecef2blh(const Vector &ecef);
Vector eci2ecef(double x, double y, double z, Vector currentTime);
Vector eci2ecef(Vector eci, Vector currentTime);

Vector ecef2eci(double x, double y, double z, Vector currentTime);

/// @brief Перевод полярных координат в декартовы
/// @param RLP Радиус, долгота, широта
Vector pol2dec(const Vector &RLP);

/// @brief Перевод декартовых координат в полярные
/// @param eci Декартовы координаты
/// @return Радиус, долгота, широта
Vector dec2pol(const Vector &eci);