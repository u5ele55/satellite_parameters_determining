#include "Core.hpp"
#include "radiotelescopes/DesignationsNoiseApplier.hpp"
#include "LinAlg.hpp"
// #include "Matrix.hpp"
#include "coordinates.hpp"
#include "time.hpp"
#include "global/Constants.hpp"
#include "integration/solver/RK4Solver.hpp"
#include "integration/system/SpacecraftECI.hpp"

#include <iostream>
void Core::start()
{
    // generateMeasurements()
    generateMeasurements();
    for(int i = 0; i < measurements.size(); i ++) {
        std::cout << times[i] << " " << measurements[i] << '\n';
    }
    // applyNoiseToState()
    // std::cout << "hey\n";
    // Matrix mtx(3,4);
    // mtx[0] = Vector{1, 2, 1, 3};
    // mtx[1] = Vector{0, 1, 0, 2};
    // mtx[2] = Vector{1, 0, 1, -1};
    // std::cout << mtx * Vector{1,1,1,1} << '\n';
    // try initial approximation 
    // .calculateResiduals()
    // make step of gradient descent / newton method
    // .step()
    // if tol < eps end
}

void Core::generateMeasurements()
{
    double angleSecond = M_PI/(180 * 3600);
    Vector telescopeBLH{30, 45, 1};

    LinAlg::toRad(telescopeBLH[0]);
    LinAlg::toRad(telescopeBLH[1]);

    RadioTelescope telescope(telescopeBLH, 7 * M_PI / 180);
    TelescopeControl radioControl(telescope);
    DesignationsNoiseApplier desNoiseApplier(radioControl, 100, 30 * angleSecond);

    Vector initialPosition = {6871257.864, 0.0, 0.0};
    Vector initialSpeed = {0.0, 3810.1125727278977, 6599.308558521686};

    Vector initialState(6);
    for(int i = 0; i < 3; i ++) {
        initialState[2 * i] = initialSpeed[i];
        initialPosition[2 * i + 1] = initialPosition[i];
    }

    auto *system = new SpacecraftECI(
        Constants::Earth::GEOCENTRIC_GRAVITATION_CONSTANT,
        Constants::Earth::ANGULAR_SPEED, 
        initialState);
    RK4Solver solver(system, 10); 

    Vector currentTime(7);

    const double JD = 2460206.383;
    const double unixTimestamp = (JD - 2440587.5) * Constants::Earth::SECONDS_IN_DAY;

    double step = 30;
    int hour = 3600;
    bool started = false;
    for (int i = 0; i <= 10 * hour; i += step) {
        double time = i;
        Vector state = solver.solve(time);
        double x = state[1], y = state[3], z = state[5];
        long long t = i + unixTimestamp;

        currentTime = unixToTime(t);
        Vector ecef = eci2ecef(x,y,z, currentTime);

        const auto& designation = desNoiseApplier.targetTelescope(ecef);
        if (!started && designation.size() == 3) {
            started = true;
        }
        if (started && designation.size() < 3) {
            break;
        }
        if (started) {
            measurements.push_back(designation);
            times.push_back(i);
        }
    }
}
