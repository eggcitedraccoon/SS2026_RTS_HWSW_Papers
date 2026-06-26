/**
 * @file Resource.h
 * @brief Defines the Resource class representing a hardware functional unit.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "OpType.h"

/**
 * @class Resource
 * @brief Represents a hardware resource instance that can execute operations.
 */
class Resource {
public:
    int id;           ///< Unique identifier for the resource instance
    OpType type;      ///< Type of operation this resource can perform
    int areaCost;     ///< Area cost associated with this resource

    /**
     * @brief Constructs a new Resource object.
     * @param id Unique ID.
     * @param type Operation type.
     * @param areaCost Area cost.
     */
    Resource(int id, OpType type, int areaCost)
        : id(id), type(type), areaCost(areaCost) {}
};

#endif // RESOURCE_H
