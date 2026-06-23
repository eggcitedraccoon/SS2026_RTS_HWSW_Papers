#include "SchedulePrinter.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

void SchedulePrinter::print(const DFG& dfg, const ScheduleState& state) {
    std::map<int, std::vector<int>> cycleToOps;
    int maxCycle = 0;
    for (auto const& [opId, cycle] : state.operationCycles) {
        cycleToOps[cycle].push_back(opId);
        maxCycle = std::max(maxCycle, cycle);
    }

    for (int c = 0; c <= maxCycle; ++c) {
        if (cycleToOps.count(c)) {
            std::cout << "Cycle " << c << " :";
            for (int opId : cycleToOps[c]) {
                auto op = dfg.getOperation(opId);
                int resId = state.resourceBindings.at(opId);
                std::cout << " " << opTypeToString(op->type) << resId << "(" << op->name << ")";
            }
            std::cout << "\n";
        }
    }
}
