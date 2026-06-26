/**
 * @file SimulatedAnnealingScheduler.cpp
 * @brief Implementation of the Simulated Annealing scheduler.
 */

#include "SimulatedAnnealingScheduler.h"
#include "NeighborGenerator.h"
#include "../cost/CostEvaluator.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>

/**
 * @brief Main SA loop.
 * 
 * Implements geometric cooling and the Metropolis acceptance criterion. 
 * Measures synthesis runtime and logs convergence data.
 */
RunResult SimulatedAnnealingScheduler::run(const DFG& dfg, const SAConfig& config, const ScheduleState& initialState, int runIndex) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ScheduleState currentState = initialState;
    NeighborGenerator generator(config.seed + runIndex);
    
    // Ensure initial state is repaired
    generator.repair(dfg, currentState);
    
    CostEvaluator::evaluate(dfg, currentState, config.alpha, config.beta, config.latencyNorm, config.areaNorm);
    
    ScheduleState bestState = currentState;
    
    double T = config.initialTemperature;

    // Per-run RNG for Metropolis acceptance. Must NOT be static — each run needs
    // its own independent stream seeded from runIndex.
    std::mt19937 acceptanceGen(config.seed + runIndex + 1000);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    RunResult result;
    result.runIndex = runIndex;

    int totalIterations = 0;
    while (T > config.minimumTemperature) {
        double sumCost = 0;
        double minCost = std::numeric_limits<double>::max();
        double maxCost = -std::numeric_limits<double>::max();

        for (int i = 0; i < config.iterationsPerTemp; ++i) {
            ScheduleState nextState = currentState;
            generator.generate(dfg, nextState);
            CostEvaluator::evaluate(dfg, nextState, config.alpha, config.beta, config.latencyNorm, config.areaNorm);
            
            double delta = nextState.cost - currentState.cost;
            
            if (delta < 0) {
                currentState = nextState;
            } else {
                double p = std::exp(-delta / T);
                if (dist(acceptanceGen) < p) {
                    currentState = nextState;
                }
            }
            
            if (currentState.cost < bestState.cost) {
                bestState = currentState;
            }
            
            sumCost += currentState.cost;
            minCost = std::min(minCost, currentState.cost);
            maxCost = std::max(maxCost, currentState.cost);

            totalIterations++;
            if (totalIterations % 100 == 0) {
                result.convergenceLog.push_back({totalIterations, currentState.cost});
                result.bestCostLog.push_back(bestState.cost);
                result.tempsLog.push_back(T);
            }
        }
        
        result.tempStats.push_back({T, sumCost / config.iterationsPerTemp, maxCost, minCost});
        T *= config.coolingRate;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.bestState = bestState;
    
    return result;
}
