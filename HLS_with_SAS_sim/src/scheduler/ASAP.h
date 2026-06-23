#ifndef ASAP_H
#define ASAP_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

class ASAP {
public:
    static bool schedule(const DFG& dfg, ScheduleState& state);
};

#endif // ASAP_H
