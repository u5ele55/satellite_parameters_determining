#include "time.hpp"

#include "../global/Constants.hpp"

#include <chrono>
#include <ctime>

#include <iostream>

Vector unixToTime(long long secs) {
    using namespace std::chrono;

    system_clock::time_point tp{seconds{secs}};
    time_t tt = system_clock::to_time_t(tp);
    tm utc_tm = *gmtime(&tt);

    Vector time = {
        (double)utc_tm.tm_year + 1900,
        (double)utc_tm.tm_mon + 1,
        (double)utc_tm.tm_mday,
        (double)utc_tm.tm_hour,
        (double)utc_tm.tm_min,
        (double)utc_tm.tm_sec,
        0
    };
    time[6] = (
        (time[3] * 60 + time[4]) * 60 + time[5]
        ) / Constants::Earth::SECONDS_IN_DAY;

    return time;
}

long long dateToUnix(Vector time) {
    std::tm dt;
    dt.tm_year = time[0] - 1900;
    dt.tm_mon  = time[1] - 1;
    dt.tm_mday = time[2];
    dt.tm_hour = time[3];
    dt.tm_min  = time[4];
    dt.tm_sec  = time[5];
    dt.tm_isdst = 0; // Not daylight saving
 
    std::time_t t = mktime(&dt); // Это "локальное время"
    std::tm dt2 = *std::gmtime(&t);

    return t + 3*60*60;
}

double dateToJDN(Vector date)
{
    int Y = date[0], M = date[1], D = date[2];
    long long JDN = 
        (1461 * (Y + 4800 + (M - 14)/12))/4 
        + (367 * (M - 2 - 12 * ((M - 14)/12)))/12 
        - (3 * ((Y + 4900 + (M - 14)/12)/100))/4 
        + D - 32075;
        
    return JDN;
}

Vector JDToTime(double JD) {
    double julian = JD + 0.5;
    long z = static_cast<long>(std::floor(julian));
    double f = julian - z;

    double a, alpha, b, c, d, e;

    if (z < 2299161) {
        a = z;
    } else {
        alpha = std::floor((z - 1867216.25) / 36524.25);
        a = z + 1 + alpha - std::floor(alpha / 4);
    }

    b = a + 1524;
    c = std::floor((b - 122.1) / 365.25);
    d = std::floor(365.25 * c);
    e = std::floor((b - d) / 30.6001);

    double day = static_cast<int>(b - d - std::floor(30.6001 * e) + f);
    double month = static_cast<int>(e < 14) ? e - 1 : e - 13;
    double year = static_cast<int>(month > 2) ? c - 4716 : c - 4715;

    double hour = static_cast<int>(f * 24);
    double minute = static_cast<int>((f * 24 - hour) * 60);
    double second = static_cast<int>( ((f * 24 - hour) * 60 - minute) * 60 );

    return {year, month, day, hour, minute, second};
}

double timeToJD(Vector date) {
    double JDN = dateToJDN(date);
    double h = date[3], m = date[4], s = date[5];
    double JD = JDN + (h-12) / 24.0 + m / 1440.0 + s / 86400.0;
    return JD;
}