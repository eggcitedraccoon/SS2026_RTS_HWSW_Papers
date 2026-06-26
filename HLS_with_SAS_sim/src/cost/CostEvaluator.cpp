/**
 * @file CostEvaluator.cpp
 * @brief Implementation of the CostEvaluator class.
 */

#include "CostEvaluator.h"
#include <algorithm>
#include <set>

/**
 * @brief Calculates cost based on latency, area, and constraint violations.
 * 
 * In addition to the standard cost formula, this method applies heavy 
 * penalties for any dependency or resource overlap violations to ensure 
 * the SA optimizer favors valid schedules.
 */
double CostEvaluator::evaluate(const DFG& dfg, ScheduleState& state, double alpha, double beta, double latencyNorm, double areaNorm) {
    int maxLatency = 0;
    for (auto const& [opId, startCycle] : state.operationCycles) {
        auto op = dfg.getOperation(opId);
        int opLatency = dfg.opTypeInfo.at(op->type).latency;
        maxLatency = std::max(maxLatency, startCycle + opLatency);
    }
    state.latency = maxLatency;

    std::set<int> usedResourceIds;
    for (auto const& [opId, resId] : state.resourceBindings) {
        usedResourceIds.insert(resId);
    }

    int totalArea = 0;
    for (int resId : usedResourceIds) {
        if (state.resources.count(resId)) {
            totalArea += state.resources.at(resId).areaCost;
        }
    }
    state.area = totalArea;

    state.cost = alpha * (static_cast<double>(state.latency) / latencyNorm) + 
                 beta * (static_cast<double>(state.area) / areaNorm);

    // Check for resource overlaps or dependency violations and add penalty if any
    // This is a safety measure.
    double penalty = 0.0;
    
    // Dependency check
    for (auto const& [opId, startCycle] : state.operationCycles) {
        auto op = dfg.getOperation(opId);
        int opLatency = dfg.opTypeInfo.at(op->type).latency;
        for (int succId : op->successors) {
            if (state.operationCycles.count(succId)) {
                if (state.operationCycles.at(succId) < startCycle + opLatency) {
                    penalty += 1000.0; // Heavy penalty
                }
            }
        }
    }

    // Resource overlap check
    std::map<int, std::vector<std::pair<int, int>>> resourceUsage; // resId -> list of [start, end)
    for (auto const& [opId, resId] : state.resourceBindings) {
        if (state.operationCycles.count(opId)) {
            auto op = dfg.getOperation(opId);
            int start = state.operationCycles.at(opId);
            int end = start + dfg.opTypeInfo.at(op->type).latency;
            resourceUsage[resId].push_back({start, end});
        }
    }

    for (auto& [resId, intervals] : resourceUsage) {
        std::sort(intervals.begin(), intervals.end());
        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i].first < intervals[i-1].second) {
                penalty += 1000.0; // Heavy penalty
            }
        }
    }
    
    // Penalty for unbound operations
    for (auto const& [opId, op] : dfg.operations) {
        if (state.resourceBindings.find(opId) == state.resourceBindings.end()) {
            penalty += 1000.0;
        }
    }

    state.cost += penalty;
    return state.cost;
}
