/**
 * @file Operation.h
 * @brief Defines the Operation class representing a node in the Data Flow Graph.
 */

#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <vector>
#include "OpType.h"

/**
 * @class Operation
 * @brief Represents an operation in the DFG, including its type, dependencies, and scheduling/binding info.
 */
class Operation {
public:
    int id;                ///< Unique identifier for the operation
    std::string name;      ///< Name of the operation (e.g., "t1", "ADD1")
    OpType type;           ///< Type of operation (LOAD, ADD, etc.)

    std::vector<int> predecessors; ///< IDs of predecessor operations
    std::vector<int> successors;   ///< IDs of successor operations

    int scheduledCycle = -1;       ///< The cycle in which this operation is scheduled
    int boundResource = -1;        ///< ID of the resource instance this operation is bound to

    /**
     * @brief Constructs a new Operation object.
     * @param id Unique ID.
     * @param name Operation name.
     * @param type Operation type.
     */
    Operation(int id, std::string name, OpType type)
        : id(id), name(name), type(type) {}
};

#endif // OPERATION_H
