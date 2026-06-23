#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <vector>
#include "OpType.h"

class Operation {
public:
    int id;
    std::string name;
    OpType type;

    std::vector<int> predecessors; // IDs of predecessor operations
    std::vector<int> successors;   // IDs of successor operations

    int scheduledCycle = -1;
    int boundResource = -1; // ID of the resource instance it's bound to

    Operation(int id, std::string name, OpType type)
        : id(id), name(name), type(type) {}
};

#endif // OPERATION_H
