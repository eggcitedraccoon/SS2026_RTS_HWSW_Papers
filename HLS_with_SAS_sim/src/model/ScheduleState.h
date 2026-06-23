#ifndef SCHEDULESTATE_H
#define SCHEDULESTATE_H

#include <vector>
#include <map>
#include "Resource.h"
#include "OpType.h"

class ScheduleState {
public:
    // operationId -> scheduledCycle
    std::map<int, int> operationCycles;
    // operationId -> resourceInstanceId
    std::map<int, int> resourceBindings;
    // resourceInstanceId -> Resource object
    std::map<int, Resource> resources;

    double cost = 0.0;
    int latency = 0;
    int area = 0;

    ScheduleState() = default;
};

#endif // SCHEDULESTATE_H
