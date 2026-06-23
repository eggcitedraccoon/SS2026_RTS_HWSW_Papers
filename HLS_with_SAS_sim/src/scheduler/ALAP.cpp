#include "ALAP.h"
#include <map>
#include <queue>
#include <algorithm>

bool ALAP::schedule(const DFG& dfg, ScheduleState& state, int maxLatency) {
    std::map<int, int> outDegree;
    std::queue<int> readyNodes;

    for (auto const& [id, op] : dfg.operations) {
        outDegree[id] = op->successors.size();
        if (outDegree[id] == 0) {
            readyNodes.push(id);
        }
    }

    while (!readyNodes.empty()) {
        int currId = readyNodes.front();
        readyNodes.pop();

        auto op = dfg.getOperation(currId);
        int opLatency = dfg.opTypeInfo.at(op->type).latency;
        
        int latestEnd = maxLatency;
        for (int succId : op->successors) {
            latestEnd = std::min(latestEnd, state.operationCycles[succId]);
        }

        state.operationCycles[currId] = latestEnd - opLatency;

        for (int predId : op->predecessors) {
            outDegree[predId]--;
            if (outDegree[predId] == 0) {
                readyNodes.push(predId);
            }
        }
    }

    return true;
}
