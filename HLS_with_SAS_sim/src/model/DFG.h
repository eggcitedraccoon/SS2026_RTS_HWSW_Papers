/**
 * @file DFG.h
 * @brief Defines the Data Flow Graph (DFG) class and related structures.
 */

#ifndef DFG_H
#define DFG_H

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "Operation.h"
#include "OpType.h"

/**
 * @struct OpInfo
 * @brief Metadata for an operation type, including its latency and area cost.
 */
struct OpInfo {
    int latency; ///< Latency in cycles
    int area;    ///< Area cost
};

/**
 * @class DFG
 * @brief Manages a collection of operations and their dependencies.
 */
class DFG {
public:
    std::map<int, std::unique_ptr<Operation>> operations; ///< Map of operation ID to Operation object
    std::map<OpType, OpInfo> opTypeInfo;                  ///< Map of operation type to its latency and area info
    
    /**
     * @brief Adds an operation to the DFG.
     * @param op A unique pointer to the Operation object.
     */
    void addOperation(std::unique_ptr<Operation> op) {
        operations[op->id] = std::move(op);
    }

    /**
     * @brief Adds a dependency edge between two operations.
     * @param fromId ID of the predecessor operation.
     * @param toId ID of the successor operation.
     */
    void addEdge(int fromId, int toId) {
        if (operations.count(fromId) && operations.count(toId)) {
            operations[fromId]->successors.push_back(toId);
            operations[toId]->predecessors.push_back(fromId);
        }
    }

    /**
     * @brief Retrieves an operation by its ID.
     * @param id The operation ID.
     * @return Pointer to the operation, or nullptr if not found.
     */
    Operation* getOperation(int id) {
        auto it = operations.find(id);
        return it != operations.end() ? it->second.get() : nullptr;
    }

    /**
     * @brief Retrieves a constant operation by its ID.
     * @param id The operation ID.
     * @return Pointer to the constant operation, or nullptr if not found.
     */
    const Operation* getOperation(int id) const {
        auto it = operations.find(id);
        return it != operations.end() ? it->second.get() : nullptr;
    }
};

#endif // DFG_H
