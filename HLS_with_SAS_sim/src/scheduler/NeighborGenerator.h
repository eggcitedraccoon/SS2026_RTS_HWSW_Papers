/**
 * @file NeighborGenerator.h
 * @brief Handles the generation of new candidate solutions for Simulated Annealing.
 */

#ifndef NEIGHBORGENERATOR_H
#define NEIGHBORGENERATOR_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include <random>

/**
 * @class NeighborGenerator
 * @brief Generates neighboring states by applying random moves and repairing constraints.
 */
class NeighborGenerator {
public:
    /**
     * @brief Constructs a NeighborGenerator with a specific seed.
     * @param seed Random seed.
     */
    NeighborGenerator(int seed) : gen(seed) {}

    /**
     * @brief Generates a new neighbor state by applying one of the move types.
     * @param dfg The Data Flow Graph.
     * @param state The current state to be modified.
     */
    void generate(const DFG& dfg, ScheduleState& state);

    /**
     * @brief Repairs a state to satisfy dependency and resource constraints.
     * @param dfg The Data Flow Graph.
     * @param state The state to be repaired.
     * @param changedOpId The ID of the operation that was recently moved (optional).
     */
    void repair(const DFG& dfg, ScheduleState& state, int changedOpId = -1);

private:
    std::mt19937 gen; ///< Random number generator

    /**
     * @brief Moves a random operation in time.
     * @param dfg DFG.
     * @param state Schedule state.
     * @param forward True to move later, false to move earlier.
     */
    void moveInTime(const DFG& dfg, ScheduleState& state, bool forward);

    /**
     * @brief Rebinds an operation to a different resource of the same type.
     * @param dfg DFG.
     * @param state Schedule state.
     */
    void rebindResource(const DFG& dfg, ScheduleState& state);

    /**
     * @brief Adds a new resource instance to the pool.
     * @param dfg DFG.
     * @param state Schedule state.
     */
    void addResource(const DFG& dfg, ScheduleState& state);

    /**
     * @brief Removes a random resource instance from the pool.
     * @param dfg DFG.
     * @param state Schedule state.
     */
    void removeResource(const DFG& dfg, ScheduleState& state);

    /**
     * @brief Recursively fixes dependency violations starting from a changed operation.
     * @param dfg DFG.
     * @param state Schedule state.
     * @param opId ID of the operation to start fixing from.
     */
    void fixDependencies(const DFG& dfg, ScheduleState& state, int opId);

    /**
     * @brief Fixes resource overlaps by shifting overlapping operations in time.
     * @param dfg DFG.
     * @param state Schedule state.
     */
    void fixResourceOverlaps(const DFG& dfg, ScheduleState& state);
};

#endif // NEIGHBORGENERATOR_H
