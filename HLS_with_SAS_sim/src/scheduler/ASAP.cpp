/**
 * @file ASAP.cpp
 * @brief Implementation of the ASAP scheduler.
 */

#include "ASAP.h"
#include <queue>
#include <algorithm>
#include <map>

/**
 * @brief Computes an ASAP schedule.
 * 
 * This implementation also performs a greedy resource binding to provide 
 * a complete initial solution for the SA engine. It ensures dependencies 
 * are respected and attempts to minimize resource overlaps.
 */
bool ASAP::schedule(const DFG& dfg, ScheduleState& state) {
    std::map<int, int> inDegree;
    std::queue<int> readyNodes;

    for (auto const& [id, op] : dfg.operations) {
        inDegree[id] = op->predecessors.size();
        if (inDegree[id] == 0) {
            readyNodes.push(id);
        }
    }

    while (!readyNodes.empty()) {
        int currId = readyNodes.front();
        readyNodes.pop();

        auto op = dfg.getOperation(currId);
        int startTime = 0;
        for (int predId : op->predecessors) {
            auto predOp = dfg.getOperation(predId);
            int predLatency = dfg.opTypeInfo.at(predOp->type).latency;
            int predStart = state.operationCycles[predId];
            startTime = std::max(startTime, predStart + predLatency);
        }

        state.operationCycles[currId] = startTime;

        // Greedy resource binding for initial solution
        // Note: This might violate resource constraints if we don't have enough resources,
        // but SA will fix it later. Actually, better to assign to first available resource.
        for (auto const& [resId, res] : state.resources) {
            if (res.type == op->type) {
                // Check if this resource is free at startTime
                bool busy = false;
                int opLatency = dfg.opTypeInfo.at(op->type).latency;
                for (auto const& [boundOpId, boundStart] : state.operationCycles) {
                    if (state.resourceBindings.count(boundOpId) && state.resourceBindings.at(boundOpId) == resId) {
                        auto boundOp = dfg.getOperation(boundOpId);
                        int boundLatency = dfg.opTypeInfo.at(boundOp->type).latency;
                        // Overlap check
                        if (!(startTime + opLatency <= boundStart || boundStart + boundLatency <= startTime)) {
                            busy = true;
                            break;
                        }
                    }
                }
                if (!busy) {
                    state.resourceBindings[currId] = resId;
                    break;
                }
            }
        }
        
        // If no resource found (e.g. initial pool too small), bind anyway to some resource
        // SA's repair logic will eventually fix it by adding resources or shifting in time.
        if (state.resourceBindings.find(currId) == state.resourceBindings.end()) {
             for (auto const& [resId, res] : state.resources) {
                if (res.type == op->type) {
                    state.resourceBindings[currId] = resId;
                    break;
                }
            }
        }

        // --- Resource Constraint Fix (Push forward if overlap occurs) ---
        if (state.resourceBindings.count(currId)) {
            int resId = state.resourceBindings[currId];
            int opLatency = dfg.opTypeInfo.at(op->type).latency;
            bool overlap = true;
            while (overlap) {
                overlap = false;
                for (auto const& [boundOpId, boundStart] : state.operationCycles) {
                    if (boundOpId == currId) continue;
                    if (state.resourceBindings.count(boundOpId) && state.resourceBindings.at(boundOpId) == resId) {
                        auto boundOp = dfg.getOperation(boundOpId);
                        int boundLatency = dfg.opTypeInfo.at(boundOp->type).latency;
                        if (!(state.operationCycles[currId] + opLatency <= boundStart || 
                              boundStart + boundLatency <= state.operationCycles[currId])) {
                            state.operationCycles[currId] = boundStart + boundLatency;
                            overlap = true;
                        }
                    }
                }
            }
        }

        for (int succId : op->successors) {
            inDegree[succId]--;
            if (inDegree[succId] == 0) {
                readyNodes.push(succId);
            }
        }
    }

    return true;
}
