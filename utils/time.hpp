#pragma once
#include "Vector.hpp"

Vector unixToTime(long long secs);
long long dateToUnix(Vector time);
double dateToJd(Vector date);