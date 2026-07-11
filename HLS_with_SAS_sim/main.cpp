/**
 * @file main.cpp
 * @brief Entry point for the HLS with SAS Simulator.
 * 
 * This program reads a DFG and a configuration file, performs initial 
 * scheduling (ASAP and List), and then runs multiple iterations of 
 * Simulated Annealing to optimize for area and latency.
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include "src/model/DFG.h"
#include "src/model/SAConfig.h"
#include "src/parser/DFGParser.h"
#include "src/parser/JSONConfigParser.h"
#include "src/scheduler/ASAP.h"
#include "src/scheduler/ALAP.h"
#include "src/scheduler/ListScheduler.h"
#include "src/cost/CostEvaluator.h"
#include "src/cost/ValidityChecker.h"
#include "src/scheduler/SimulatedAnnealingScheduler.h"
#include "src/output/CsvExporter.h"
#include "src/output/SchedulePrinter.h"

/**
 * @brief Calculates and prints statistics for multiple SA runs.
 * @param results A vector of results from independent runs.
 */
void printStatistics(const std::vector<RunResult>& results) {
    if (results.empty()) return;
    
    std::vector<double> costs;
    for (const auto& r : results) costs.push_back(r.bestState.cost);
    
    double sum = std::accumulate(costs.begin(), costs.end(), 0.0);
    double mean = sum / costs.size();
    
    double sq_sum = std::inner_product(costs.begin(), costs.end(), costs.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / costs.size() - mean * mean);
    
    auto [min_it, max_it] = std::minmax_element(costs.begin(), costs.end());
    
    std::sort(costs.begin(), costs.end());
    double median = costs[costs.size() / 2];

    std::cout << "\n=== Multi-Run Statistics (" << results.size() << " runs) ===\n";
    std::cout << "Best Cost:   " << *min_it << "\n";
    std::cout << "Worst Cost:  " << *max_it << "\n";
    std::cout << "Average:     " << mean << "\n";
    std::cout << "Median:      " << median << "\n";
    std::cout << "Std Dev:     " << stdev << "\n";
}

int main(int argc, char* argv[]) {
    std::string dfgFile = "example.dfg";
    std::string configFile = "config.json";
    bool randomInitRequested = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dfg" && i + 1 < argc) dfgFile = argv[++i];
        else if (arg == "--config" && i + 1 < argc) configFile = argv[++i];
        else if (arg == "--random-init") randomInitRequested = true;
    }

    DFG dfg;
    if (!DFGParser::parseDFG(dfgFile, dfg)) {
        std::cerr << "Failed to parse DFG: " << dfgFile << std::endl;
        return 1;
    }

    SAConfig config;
    ScheduleState initialState;
    if (!JSONConfigParser::parseConfig(configFile, config, dfg, initialState)) {
        std::cerr << "Failed to parse config: " << configFile << ". Using defaults.\n";
    }
    
    if (randomInitRequested) config.randomInit = true;

    // Manual override from CLI could be added here

    std::cout << "Initial scheduling (ASAP)..." << std::endl;
    ASAP::schedule(dfg, initialState);
    // Use initial ASAP results for normalization
    CostEvaluator::evaluate(dfg, initialState, config.alpha, config.beta); 
    config.latencyNorm = std::max(1.0, static_cast<double>(initialState.latency));
    config.areaNorm = std::max(1.0, static_cast<double>(initialState.area));

    double asapCost = CostEvaluator::evaluate(dfg, initialState, config.alpha, config.beta, config.latencyNorm, config.areaNorm);
    std::cout << "ASAP Cost: " << asapCost << " (Lat: " << initialState.latency << ", Area: " << initialState.area << ")\n";

    ScheduleState listState;
    listState.resources = initialState.resources;
    ListScheduler::schedule(dfg, listState);
    double listCost = CostEvaluator::evaluate(dfg, listState, config.alpha, config.beta, config.latencyNorm, config.areaNorm);
    std::cout << "List Scheduler Cost: " << listCost << " (Lat: " << listState.latency << ", Area: " << listState.area << ")\n";
    
    std::cout << "Starting SA Optimization (" << config.runs << " runs)" << (config.randomInit ? " with random initialization" : "") << "..." << std::endl;
    
    std::vector<RunResult> results;
    for (int i = 0; i < config.runs; ++i) {
        std::cout << "Run " << i+1 << "/" << config.runs << "..." << std::flush;
        auto result = SimulatedAnnealingScheduler::run(dfg, config, initialState, i + 1);
        results.push_back(result);
        std::cout << " Done. Cost: " << result.bestState.cost << " Time: " << result.executionTimeMs << "ms" << std::endl;
    }

    printStatistics(results);

    // Find overall best run
    auto bestRunIt = std::min_element(results.begin(), results.end(), [](const RunResult& a, const RunResult& b) {
        return a.bestState.cost < b.bestState.cost;
    });

    std::cout << "\n=== Best Solution Found (Run " << bestRunIt->runIndex << ") ===\n";
    SchedulePrinter::print(dfg, bestRunIt->bestState);
    std::cout << "Final Cost: " << bestRunIt->bestState.cost << " (Latency: " << bestRunIt->bestState.latency << ", Area: " << bestRunIt->bestState.area << ")\n";

    // Export results
    std::string outputDir = "results";
    #ifdef _WIN32
        _mkdir(outputDir.c_str());
    #else
        mkdir(outputDir.c_str(), 0777);
    #endif

    CsvExporter::exportRunSummary(outputDir + "/run_summary.csv", results);
    CsvExporter::exportCostFunctionParams(outputDir + "/cost_function.csv", config.alpha, config.beta,
                                           config.latencyNorm, config.areaNorm);
    
    for (const auto& res : results) {
        std::string runPrefix = outputDir + "/run_" + std::to_string(res.runIndex);
        CsvExporter::exportTempStats(runPrefix + "_temp_stats.csv", res.tempStats);
        CsvExporter::exportConvergenceLog(runPrefix + "_convergence.csv", res, res.tempsLog);
        CsvExporter::exportAcceptedTrace(runPrefix + "_accepted_trace.csv", res.acceptedTrace);
    }
    
    CsvExporter::exportGantt(outputDir + "/best_gantt.csv", dfg, bestRunIt->bestState);
    
    std::cout << "\nResults exported to " << outputDir << "/ directory" << std::endl;

    return 0;
}
