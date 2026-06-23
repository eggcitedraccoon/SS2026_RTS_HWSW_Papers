#ifndef DFG_H
#define DFG_H

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "Operation.h"
#include "OpType.h"

struct OpInfo {
    int latency;
    int area;
};

class DFG {
public:
    std::map<int, std::unique_ptr<Operation>> operations;
    std::map<OpType, OpInfo> opTypeInfo;
    
    void addOperation(std::unique_ptr<Operation> op) {
        operations[op->id] = std::move(op);
    }

    void addEdge(int fromId, int toId) {
        if (operations.count(fromId) && operations.count(toId)) {
            operations[fromId]->successors.push_back(toId);
            operations[toId]->predecessors.push_back(fromId);
        }
    }

    Operation* getOperation(int id) {
        auto it = operations.find(id);
        return it != operations.end() ? it->second.get() : nullptr;
    }

    const Operation* getOperation(int id) const {
        auto it = operations.find(id);
        return it != operations.end() ? it->second.get() : nullptr;
    }
};

#endif // DFG_H
