/**
 * @file ValidityChecker.h
 * @brief Utility for verifying that a schedule satisfies all constraints.
 */

#ifndef VALIDITYCHECKER_H
#define VALIDITYCHECKER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include <iostream>

/**
 * @class ValidityChecker
 * @brief Provides a strict verification of dependency and resource constraints.
 */
class ValidityChecker {
public:
    /**
     * @brief Checks if the given state is valid.
     * @param dfg The Data Flow Graph.
     * @param state The state to check.
     * @param verbose If true, prints details about violations to stdout.
     * @return true if valid, false otherwise.
     */
    static bool check(const DFG& dfg, const ScheduleState& state, bool verbose = false) {
        bool valid = true;

        // Dependency check
        for (auto const& [id, op] : dfg.operations) {
            int start = state.operationCycles.at(id);
            int latency = dfg.opTypeInfo.at(op->type).latency;
            for (int succId : op->successors) {
                int succStart = state.operationCycles.at(succId);
                if (succStart < start + latency) {
                    if (verbose) {
                        std::cout << "Violation: " << op->name << " (end " << start + latency 
                                  << ") -> " << dfg.getOperation(succId)->name << " (start " << succStart << ")\n";
                    }
                    valid = false;
                }
            }
        }

        // Resource overlap check
        std::map<int, std::vector<std::pair<int, int>>> resUsage;
        for (auto const& [opId, resId] : state.resourceBindings) {
            auto op = dfg.getOperation(opId);
            int start = state.operationCycles.at(opId);
            int end = start + dfg.opTypeInfo.at(op->type).latency;
            resUsage[resId].push_back({start, end});
        }

        for (auto& [resId, intervals] : resUsage) {
            std::sort(intervals.begin(), intervals.end());
            for (size_t i = 1; i < intervals.size(); ++i) {
                if (intervals[i].first < intervals[i-1].second) {
                    if (verbose) {
                        std::cout << "Overlap on Res " << resId << ": [" << intervals[i-1].first << "," << intervals[i-1].second 
                                  << ") and [" << intervals[i].first << "," << intervals[i].second << ")\n";
                    }
                    valid = false;
                }
            }
        }
        
        // Unbound ops check
        for (auto const& [id, op] : dfg.operations) {
            if (state.resourceBindings.find(id) == state.resourceBindings.end()) {
                if (verbose) std::cout << "Op " << op->name << " is unbound!\n";
                valid = false;
            }
        }

        return valid;
    }
};

#endif // VALIDITYCHECKER_H
