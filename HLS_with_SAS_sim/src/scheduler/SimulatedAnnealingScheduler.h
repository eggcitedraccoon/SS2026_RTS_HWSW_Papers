#ifndef SIMULATEDANNEALINGSCHEDULER_H
#define SIMULATEDANNEALINGSCHEDULER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include "../model/SAConfig.h"
#include <vector>

struct RunResult {
    int runIndex;
    ScheduleState bestState;
    double executionTimeMs;
    std::vector<std::pair<int, double>> convergenceLog; // iteration, best_cost
    std::vector<double> tempsLog;
};

class SimulatedAnnealingScheduler {
public:
    static RunResult run(const DFG& dfg, const SAConfig& config, const ScheduleState& initialState, int runIndex);
};

#endif // SIMULATEDANNEALINGSCHEDULER_H
