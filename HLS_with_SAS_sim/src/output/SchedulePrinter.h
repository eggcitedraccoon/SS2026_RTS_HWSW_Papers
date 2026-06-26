/**
 * @file SchedulePrinter.h
 * @brief Provides a text-based visualization of the operation schedule.
 */

#ifndef SCHEDULEPRINTER_H
#define SCHEDULEPRINTER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"

/**
 * @class SchedulePrinter
 * @brief Static class for printing the schedule to the console.
 */
class SchedulePrinter {
public:
    /**
     * @brief Prints the schedule in a cycle-by-cycle text format.
     * @param dfg The Data Flow Graph.
     * @param state The schedule state to print.
     */
    static void print(const DFG& dfg, const ScheduleState& state);
};

#endif // SCHEDULEPRINTER_H
