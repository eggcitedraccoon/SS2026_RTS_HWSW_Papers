#ifndef SCHEDULEPRINTER_H
#define SCHEDULEPRINTER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

class SchedulePrinter {
public:
    static void print(const DFG& dfg, const ScheduleState& state);
};

#endif // SCHEDULEPRINTER_H
