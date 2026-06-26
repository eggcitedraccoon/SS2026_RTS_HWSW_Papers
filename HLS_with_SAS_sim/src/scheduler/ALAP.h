/**
 * @file ALAP.h
 * @brief Provides the As-Late-As-Possible (ALAP) scheduling algorithm.
 */

#ifndef ALAP_H
#define ALAP_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

/**
 * @class ALAP
 * @brief Static class for ALAP scheduling.
 */
class ALAP {
public:
    /**
     * @brief Performs ALAP scheduling on the given DFG.
     * @param dfg The Data Flow Graph.
     * @param state The ScheduleState to be populated with cycles.
     * @param maxLatency The maximum allowed latency (deadlines).
     * @return true if successful.
     */
    static bool schedule(const DFG& dfg, ScheduleState& state, int maxLatency);
};

#endif // ALAP_H
