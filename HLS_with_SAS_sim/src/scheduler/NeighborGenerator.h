#ifndef NEIGHBORGENERATOR_H
#define NEIGHBORGENERATOR_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include <random>

class NeighborGenerator {
public:
    NeighborGenerator(int seed) : gen(seed) {}

    void generate(const DFG& dfg, ScheduleState& state);
    void repair(const DFG& dfg, ScheduleState& state, int changedOpId = -1);

private:
    std::mt19937 gen;

    void moveInTime(const DFG& dfg, ScheduleState& state, bool forward);
    void rebindResource(const DFG& dfg, ScheduleState& state);
    void addResource(const DFG& dfg, ScheduleState& state);
    void removeResource(const DFG& dfg, ScheduleState& state);

    void fixDependencies(const DFG& dfg, ScheduleState& state, int opId);
    void fixResourceOverlaps(const DFG& dfg, ScheduleState& state);
};

#endif // NEIGHBORGENERATOR_H
