/**
 * @file ASAP.h
 * @brief Provides the As-Soon-As-Possible (ASAP) scheduling algorithm.
 */

#ifndef ASAP_H
#define ASAP_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

/**
 * @class ASAP
 * @brief Static class for ASAP scheduling.
 */
class ASAP {
public:
    /**
     * @brief Performs ASAP scheduling on the given DFG and updates the state.
     * @param dfg The Data Flow Graph.
     * @param state The ScheduleState to be populated with cycles and bindings.
     * @return true if successful.
     */
    static bool schedule(const DFG& dfg, ScheduleState& state);
};

#endif // ASAP_H
