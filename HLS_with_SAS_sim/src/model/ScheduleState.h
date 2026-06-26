/**
 * @file ScheduleState.h
 * @brief Represents a candidate solution for the HLS scheduling and binding problem.
 */

#ifndef SCHEDULESTATE_H
#define SCHEDULESTATE_H

#include <vector>
#include <map>
#include "Resource.h"
#include "OpType.h"

/**
 * @class ScheduleState
 * @brief Stores the scheduling cycles, resource bindings, and evaluated cost of a solution.
 */
class ScheduleState {
public:
    std::map<int, int> operationCycles;  ///< Map of operation ID to its scheduled start cycle
    std::map<int, int> resourceBindings; ///< Map of operation ID to the resource instance ID it's bound to
    std::map<int, Resource> resources;   ///< Map of resource instance ID to Resource object (allocated resources)

    double cost = 0.0;                   ///< Evaluated cost: alpha * latency + beta * area
    int latency = 0;                     ///< Total latency of the schedule
    int area = 0;                        ///< Total area cost of the allocated resources

    /**
     * @brief Default constructor.
     */
    ScheduleState() = default;
};

#endif // SCHEDULESTATE_H
