#ifndef ALAP_H
#define ALAP_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

class ALAP {
public:
    static bool schedule(const DFG& dfg, ScheduleState& state, int maxLatency);
};

#endif // ALAP_H
