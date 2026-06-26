/**
 * @file ListScheduler.h
 * @brief Provides a priority-based list scheduling algorithm.
 */

#ifndef LISTSCHEDULER_H
#define LISTSCHEDULER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

/**
 * @class ListScheduler
 * @brief Static class for List Scheduling.
 */
class ListScheduler {
public:
    /**
     * @brief Performs list scheduling on the given DFG.
     * @param dfg The Data Flow Graph.
     * @param state The ScheduleState to be populated.
     * @return true if successful.
     */
    static bool schedule(const DFG& dfg, ScheduleState& state);
};

#endif // LISTSCHEDULER_H
