#ifndef RESOURCE_H
#define RESOURCE_H

#include "OpType.h"

class Resource {
public:
    int id;
    OpType type;
    int areaCost;

    Resource(int id, OpType type, int areaCost)
        : id(id), type(type), areaCost(areaCost) {}
};

#endif // RESOURCE_H
