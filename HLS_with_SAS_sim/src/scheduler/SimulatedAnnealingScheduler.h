/**
 * @file SimulatedAnnealingScheduler.h
 * @brief Defines the Simulated Annealing (SA) scheduler and the structure for its results.
 */

#ifndef SIMULATEDANNEALINGSCHEDULER_H
#define SIMULATEDANNEALINGSCHEDULER_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include "../model/SAConfig.h"
#include <vector>

/**
 * @struct TempStats
 * @brief Records, for a single temperature level, the cost of the solution the
 * run actually carries forward into the next (cooler) level.
 */
struct TempStats {
    double temperature;
    double resultingCost;
};

/**
 * @struct RunResult
 * @brief Stores the outcome of a single Simulated Annealing execution.
 */
struct RunResult {
    int runIndex;                                   ///< Index of the run
    ScheduleState bestState;                        ///< The best solution found during the run
    double executionTimeMs;                         ///< Runtime of the synthesis in milliseconds
    std::vector<std::pair<int, double>> convergenceLog; ///< Log of (iteration, best_cost) for plotting
    std::vector<double> bestCostLog;                ///< Best-so-far cost, sampled in sync with convergenceLog
    std::vector<double> tempsLog;                   ///< Log of temperature over iterations
    std::vector<TempStats> tempStats;               ///< Statistics per temperature level
};

/**
 * @class SimulatedAnnealingScheduler
 * @brief Orchestrates the Simulated Annealing process for HLS.
 */
class SimulatedAnnealingScheduler {
public:
    /**
     * @brief Executes one run of the Simulated Annealing algorithm.
     * @param dfg The Data Flow Graph.
     * @param config The SA configuration parameters.
     * @param initialState The starting candidate solution.
     * @param runIndex The index of this run.
     * @return A RunResult object containing the best state and performance metrics.
     */
    static RunResult run(const DFG& dfg, const SAConfig& config, const ScheduleState& initialState, int runIndex);
};

#endif // SIMULATEDANNEALINGSCHEDULER_H
