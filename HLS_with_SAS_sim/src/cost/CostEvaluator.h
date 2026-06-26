/**
 * @file CostEvaluator.h
 * @brief Provides functionality to calculate the cost of a candidate solution.
 */

#ifndef COSTEVALUATOR_H
#define COSTEVALUATOR_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include "../model/SAConfig.h"

/**
 * @class CostEvaluator
 * @brief Static class for evaluating the cost function (alpha * latency + beta * area).
 */
class CostEvaluator {
public:
    /**
     * @brief Evaluates the cost of the given schedule state.
     * @param dfg The Data Flow Graph.
     * @param state The state to be evaluated.
     * @param alpha Weight for latency.
     * @param beta Weight for area.
     * @param latencyNorm Normalization factor for latency.
     * @param areaNorm Normalization factor for area.
     * @return The calculated cost including any penalties for constraint violations.
     */
    static double evaluate(const DFG& dfg, ScheduleState& state, double alpha, double beta, double latencyNorm = 1.0, double areaNorm = 1.0);
};

#endif // COSTEVALUATOR_H
