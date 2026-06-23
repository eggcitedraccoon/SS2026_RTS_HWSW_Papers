#include "NeighborGenerator.h"
#include <algorithm>
#include <iostream>

void NeighborGenerator::generate(const DFG& dfg, ScheduleState& state) {
    std::uniform_int_distribution<int> moveDist(1, 5);
    int moveType = moveDist(gen);

    switch (moveType) {
        case 1: moveInTime(dfg, state, true); break;
        case 2: moveInTime(dfg, state, false); break;
        case 3: rebindResource(dfg, state); break;
        case 4: addResource(dfg, state); break;
        case 5: removeResource(dfg, state); break;
    }
}

void NeighborGenerator::moveInTime(const DFG& dfg, ScheduleState& state, bool forward) {
    if (dfg.operations.empty()) return;
    std::uniform_int_distribution<int> opDist(0, dfg.operations.size() - 1);
    auto it = dfg.operations.begin();
    std::advance(it, opDist(gen));
    int opId = it->first;

    if (forward) {
        state.operationCycles[opId] += 1;
    } else {
        state.operationCycles[opId] = std::max(0, state.operationCycles[opId] - 1);
        // Note: Moving backward might violate predecessors. 
        // Our repair logic should ideally handle both directions, 
        // but backward movement might be tricky if we only repair "forward".
        // If we move backward and it violates pred, we should push it forward again? 
        // That would undo the move. 
        // Actually, "soft repair" usually pushes forward. 
    }
    repair(dfg, state, opId);
}

void NeighborGenerator::rebindResource(const DFG& dfg, ScheduleState& state) {
    if (dfg.operations.empty()) return;
    std::uniform_int_distribution<int> opDist(0, dfg.operations.size() - 1);
    auto it = dfg.operations.begin();
    std::advance(it, opDist(gen));
    int opId = it->first;
    OpType type = it->second->type;

    std::vector<int> compatibleResources;
    for (auto const& [resId, res] : state.resources) {
        if (res.type == type) {
            compatibleResources.push_back(resId);
        }
    }

    if (!compatibleResources.empty()) {
        std::uniform_int_distribution<int> resDist(0, compatibleResources.size() - 1);
        state.resourceBindings[opId] = compatibleResources[resDist(gen)];
        repair(dfg, state, opId);
    }
}

void NeighborGenerator::addResource(const DFG& dfg, ScheduleState& state) {
    // Pick a random OpType from the DFG
    std::vector<OpType> types;
    for (auto const& [type, info] : dfg.opTypeInfo) types.push_back(type);
    if (types.empty()) return;

    std::uniform_int_distribution<int> typeDist(0, types.size() - 1);
    OpType selectedType = types[typeDist(gen)];

    int newId = 0;
    if (!state.resources.empty()) {
        newId = state.resources.rbegin()->first + 1;
    }
    state.resources.emplace(newId, Resource(newId, selectedType, dfg.opTypeInfo.at(selectedType).area));
    // No repair needed for adding resource, but cost changes.
}

void NeighborGenerator::removeResource(const DFG& dfg, ScheduleState& state) {
    if (state.resources.size() <= 1) return; // Keep at least one? Or at least one of each type?
    
    // Better: ensure at least one resource of each type used in DFG remains.
    std::map<OpType, std::vector<int>> resourcesByType;
    for (auto const& [resId, res] : state.resources) {
        resourcesByType[res.type].push_back(resId);
    }

    std::vector<int> removableResources;
    for (auto const& [type, ids] : resourcesByType) {
        if (ids.size() > 1) {
            for (int id : ids) removableResources.push_back(id);
        }
    }

    if (removableResources.empty()) return;

    std::uniform_int_distribution<int> resDist(0, removableResources.size() - 1);
    int resIdToRemove = removableResources[resDist(gen)];
    OpType removedType = state.resources.at(resIdToRemove).type;

    state.resources.erase(resIdToRemove);

    // Rebind operations that were using this resource
    int fallbackResId = resourcesByType[removedType][0] == resIdToRemove ? resourcesByType[removedType][1] : resourcesByType[removedType][0];
    
    for (auto& [opId, boundResId] : state.resourceBindings) {
        if (boundResId == resIdToRemove) {
            boundResId = fallbackResId;
        }
    }
    repair(dfg, state);
}

void NeighborGenerator::repair(const DFG& dfg, ScheduleState& state, int changedOpId) {
    // 1. Fix dependencies (recursive shift forward)
    if (changedOpId != -1) {
        fixDependencies(dfg, state, changedOpId);
    } else {
        // Full dependency check if no specific op changed
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto const& [id, op] : dfg.operations) {
                int start = state.operationCycles[id];
                int latency = dfg.opTypeInfo.at(op->type).latency;
                for (int succId : op->successors) {
                    if (state.operationCycles[succId] < start + latency) {
                        state.operationCycles[succId] = start + latency;
                        changed = true;
                    }
                }
            }
        }
    }

    // 2. Fix resource overlaps
    fixResourceOverlaps(dfg, state);
}

void NeighborGenerator::fixDependencies(const DFG& dfg, ScheduleState& state, int opId) {
    auto op = dfg.getOperation(opId);
    int start = state.operationCycles[opId];
    int latency = dfg.opTypeInfo.at(op->type).latency;

    for (int succId : op->successors) {
        if (state.operationCycles[succId] < start + latency) {
            state.operationCycles[succId] = start + latency;
            fixDependencies(dfg, state, succId);
        }
    }
    
    // Also check predecessors: if move backward caused violation, push this op forward
    int earliestStart = 0;
    for (int predId : op->predecessors) {
        auto predOp = dfg.getOperation(predId);
        earliestStart = std::max(earliestStart, state.operationCycles[predId] + dfg.opTypeInfo.at(predOp->type).latency);
    }
    if (state.operationCycles[opId] < earliestStart) {
        state.operationCycles[opId] = earliestStart;
        fixDependencies(dfg, state, opId); // Re-propagate
    }
}

void NeighborGenerator::fixResourceOverlaps(const DFG& dfg, ScheduleState& state) {
    bool globallyChanged = true;
    while (globallyChanged) {
        globallyChanged = false;
        
        std::map<int, std::vector<int>> resToOps;
        for (auto const& [opId, resId] : state.resourceBindings) {
            resToOps[resId].push_back(opId);
        }

        for (auto& [resId, opIds] : resToOps) {
            // Sort ops by start cycle
            std::sort(opIds.begin(), opIds.end(), [&](int a, int b) {
                return state.operationCycles[a] < state.operationCycles[b];
            });

            for (size_t i = 1; i < opIds.size(); ++i) {
                int prevOpId = opIds[i-1];
                int currOpId = opIds[i];
                
                int prevStart = state.operationCycles[prevOpId];
                int prevLatency = dfg.opTypeInfo.at(dfg.getOperation(prevOpId)->type).latency;
                
                if (state.operationCycles[currOpId] < prevStart + prevLatency) {
                    state.operationCycles[currOpId] = prevStart + prevLatency;
                    fixDependencies(dfg, state, currOpId);
                    globallyChanged = true;
                }
            }
        }
    }
}
