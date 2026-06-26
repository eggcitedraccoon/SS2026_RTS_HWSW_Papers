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
     * @brief Exports a Gantt-style CSV for schedule visualization.
     * @param filename Path to the output CSV file.
     * @param dfg The Data Flow Graph.
     * @param state The best found schedule state.
     */
    static void exportGantt(const std::string& filename, const DFG& dfg, const ScheduleState& state);
};

#endif // CSVEXPORTER_H
