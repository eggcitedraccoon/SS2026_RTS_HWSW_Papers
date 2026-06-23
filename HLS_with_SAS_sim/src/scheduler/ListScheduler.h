#ifndef LISTSCHEDULER_H
#define LISTSCHEDULER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

class ListScheduler {
public:
    static bool schedule(const DFG& dfg, ScheduleState& state);
};

#endif // LISTSCHEDULER_H
