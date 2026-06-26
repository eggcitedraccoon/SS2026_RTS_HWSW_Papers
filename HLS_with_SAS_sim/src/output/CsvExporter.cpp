/**
 * @file CsvExporter.cpp
 * @brief Implementation of the CsvExporter class.
 */

#include "CsvExporter.h"
#include <fstream>
#include <iostream>

/**
 * @brief Writes summary statistics for multiple runs to a CSV file.
 */
void CsvExporter::exportRunSummary(const std::string& filename, const std::vector<RunResult>& results) {
    std::ofstream f(filename);
    if (!f.is_open()) return;
    f << "run,cost,latency,area,time_ms\n";
    for (const auto& res : results) {
        f << res.runIndex << "," << res.bestState.cost << "," << res.bestState.latency << "," << res.bestState.area << "," << res.executionTimeMs << "\n";
    }
}

void CsvExporter::exportConvergenceLog(const std::string& filename, const RunResult& result, const std::vector<double>& temps) {
    std::ofstream f(filename);
    if (!f.is_open()) return;
    f << "iteration,temp,current_cost,best_cost\n";
    for (size_t i = 0; i < result.convergenceLog.size(); ++i) {
        double t = (i < temps.size()) ? temps[i] : 0.0;
        double bestCost = (i < result.bestCostLog.size())
                              ? result.bestCostLog[i]
                              : result.convergenceLog[i].second;
        f << result.convergenceLog[i].first << ","
          << t << ","
          << result.convergenceLog[i].second << ","
          << bestCost << "\n";
    }
}

void CsvExporter::exportTempStats(const std::string& filename, const std::vector<TempStats>& stats) {
    std::ofstream f(filename);
    if (!f.is_open()) return;
    f << "temperature,avg_cost,max_cost,min_cost\n";
    for (const auto& s : stats) {
        f << s.temperature << "," << s.avgCost << "," << s.maxCost << "," << s.minCost << "\n";
    }
}

void CsvExporter::exportGantt(const std::string& filename, const DFG& dfg, const ScheduleState& state) {
    std::ofstream f(filename);
    if (!f.is_open()) return;
    f << "resource,start,end,operation\n";
    for (auto const& [opId, resId] : state.resourceBindings) {
        auto op = dfg.getOperation(opId);
        int start = state.operationCycles.at(opId);
        int end = start + dfg.opTypeInfo.at(op->type).latency;
        f << "Res_" << resId << "_" << opTypeToString(state.resources.at(resId).type) << "," << start << "," << end << "," << op->name << "\n";
    }
}
