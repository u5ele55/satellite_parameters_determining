#pragma once
#include "Vector.hpp"

Vector unixToTime(long long secs);
long long dateToUnix(Vector time);
double dateToJDN(Vector date);
double timeToJD(Vector date);
Vector JDToTime(double JD);