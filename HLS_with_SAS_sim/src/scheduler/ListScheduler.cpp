#include "ListScheduler.h"
#include "ASAP.h"
#include "ALAP.h"
#include <vector>
#include <algorithm>
#include <map>
#include <set>

bool ListScheduler::schedule(const DFG& dfg, ScheduleState& state) {
    // 1. Calculate ASAP and ALAP to get priorities
    ScheduleState asapState;
    asapState.resources = state.resources;
    ASAP::schedule(dfg, asapState);
    
    int maxL = 0;
    for (auto const& [id, c] : asapState.operationCycles) {
        maxL = std::max(maxL, c + dfg.opTypeInfo.at(dfg.getOperation(id)->type).latency);
    }
    
    ScheduleState alapState;
    alapState.resources = state.resources;
    ALAP::schedule(dfg, alapState, maxL);
    
    // priority: smaller mobility (ALAP - ASAP) is better
    struct OpPriority {
        int id;
        int mobility;
    };
    std::vector<OpPriority> priorities;
    for (auto const& [id, op] : dfg.operations) {
        priorities.push_back({id, alapState.operationCycles[id] - asapState.operationCycles[id]});
    }
    
    // 2. Schedule cycle by cycle
    std::map<int, int> inDegree;
    for (auto const& [id, op] : dfg.operations) inDegree[id] = op->predecessors.size();
    
    std::set<int> scheduled;
    int currentCycle = 0;
    
    // resourceId -> cycle when it becomes free
    std::map<int, int> resourceFreeTime;
    for (auto const& [resId, res] : state.resources) resourceFreeTime[resId] = 0;

    while (scheduled.size() < dfg.operations.size()) {
        std::vector<int> ready;
        for (auto const& [id, deg] : inDegree) {
            if (deg == 0 && scheduled.find(id) == scheduled.end()) {
                ready.push_back(id);
            }
        }
        
        // Sort ready by mobility
        std::sort(ready.begin(), ready.end(), [&](int a, int b) {
            int mobA = alapState.operationCycles[a] - asapState.operationCycles[a];
            int mobB = alapState.operationCycles[b] - asapState.operationCycles[b];
            return mobA < mobB;
        });
        
        // Find free resource of correct type
        for (int opId : ready) {
            auto op = dfg.getOperation(opId);
            // Check if all predecessors are finished
            bool predsFinished = true;
            for (int predId : op->predecessors) {
                auto predOp = dfg.getOperation(predId);
                int predLat = dfg.opTypeInfo.at(predOp->type).latency;
                if (state.operationCycles.find(predId) == state.operationCycles.end() || 
                    state.operationCycles.at(predId) + predLat > currentCycle) {
                    predsFinished = false;
                    break;
                }
            }
            if (!predsFinished) continue;

            int assignedRes = -1;
            for (auto const& [resId, res] : state.resources) {
                if (res.type == op->type && resourceFreeTime[resId] <= currentCycle) {
                    assignedRes = resId;
                    break;
                }
            }
            
            if (assignedRes != -1) {
                state.operationCycles[opId] = currentCycle;
                state.resourceBindings[opId] = assignedRes;
                resourceFreeTime[assignedRes] = currentCycle + dfg.opTypeInfo.at(op->type).latency;
                scheduled.insert(opId);
                for (int succId : op->successors) inDegree[succId]--;
            }
        }
        currentCycle++;
        if (currentCycle > 10000) break; // Safety break
    }

    return true;
}
