/**
 * @file CsvExporter.h
 * @brief Provides functionality to export synthesis results and logs to CSV format.
 */

#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <string>
#include <vector>
#include "../scheduler/SimulatedAnnealingScheduler.h"

/**
 * @class CsvExporter
 * @brief Static class for exporting results to CSV files.
 */
class CsvExporter {
public:
    /**
     * @brief Exports a summary of all SA runs.
     * @param filename Path to the output CSV file.
     * @param results Vector of RunResult objects.
     */
    static void exportRunSummary(const std::string& filename, const std::vector<RunResult>& results);

    /**
     * @brief Exports the convergence log (iteration, temp, best_cost) for a specific run.
     * @param filename Path to the output CSV file.
     * @param result The RunResult object.
     * @param temps Vector of temperature values over iterations.
     */
    static void exportConvergenceLog(const std::string& filename, const RunResult& result, const std::vector<double>& temps);

    /**
     * @brief Exports the temperature-based statistics for a run.
     * @param filename Path to the output CSV file.
     * @param stats Vector of TempStats.
     */
    static void exportTempStats(const std::string& filename, const std::vector<TempStats>& stats);

    /**
     * @brief Exports every accepted solution along a run's search trajectory
     * (iteration, temperature, latency, area, cost), for 3D cost-surface plotting.
     * @param filename Path to the output CSV file.
     * @param trace Vector of AcceptedPoint.
     */
    static void exportAcceptedTrace(const std::string& filename, const std::vector<AcceptedPoint>& trace);

    /**
     * @brief Exports the alpha/beta weights and the latency/area normalization
     * factors (derived from the ASAP baseline) used to compute cost, so external
     * tools can reconstruct the exact cost function.
     * @param filename Path to the output CSV file.
     * @param alpha Latency weight.
     * @param beta Area weight.
     * @param latencyNorm Latency normalization factor.
     * @param areaNorm Area normalization factor.
     */
    static void exportCostFunctionParams(const std::string& filename, double alpha, double beta,
                                          double latencyNorm, double areaNorm);

    /**
     * @brief Exports a Gantt-style CSV for schedule visualization.
     * @param filename Path to the output CSV file.
     * @param dfg The Data Flow Graph.
     * @param state The best found schedule state.
     */
    static void exportGantt(const std::string& filename, const DFG& dfg, const ScheduleState& state);
};

#endif // CSVEXPORTER_H
