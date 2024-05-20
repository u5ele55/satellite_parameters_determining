#include "AbstractSolver.hpp"

AbstractSolver::AbstractSolver(ISystem *system, double step)
    : system(system), step(step) {
        states.push_back(system->getInitialState());
    }

Vector AbstractSolver::solve(double time)
{
    if (system == nullptr) {
        return Vector(0);
    }
    Vector state(states[0].size());
    int stepsInTime = static_cast<int>(time / step);

    if (time < 0) {
        state = states[0];
        for(int i = states.size() - 1; i < -stepsInTime; i++) {
            methodStep(state, -step, i * step);
        }
        double wholePart = step * stepsInTime;
        double timeDiff = time - wholePart;

        if (timeDiff != 0) {
            methodStep(state, timeDiff, time);
        }

        return state;
    } 

    if (time < (states.size() - 1) * step) {
        state = states[stepsInTime];
    } else {
        state = states[states.size() - 1];
        for(int i = states.size() - 1; i < stepsInTime; i++) {
            methodStep(state, step, i * step);
            states.push_back(state);
        }
    }

    // recalculate more precisely for case when t != step * k for some whole k
    double wholePart = step * stepsInTime;
    double timeDiff = time - wholePart;

    if (timeDiff != 0) {
        methodStep(state, timeDiff, time);
    }

    return state;
}