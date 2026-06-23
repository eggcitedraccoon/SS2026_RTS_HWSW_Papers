#include "SimulatedAnnealingScheduler.h"
#include "NeighborGenerator.h"
#include "../cost/CostEvaluator.h"
#include <chrono>
#include <cmath>
#include <iostream>

RunResult SimulatedAnnealingScheduler::run(const DFG& dfg, const SAConfig& config, const ScheduleState& initialState, int runIndex) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ScheduleState currentState = initialState;
    NeighborGenerator generator(config.seed + runIndex);
    
    // Ensure initial state is repaired
    generator.repair(dfg, currentState);
    
    CostEvaluator::evaluate(dfg, currentState, config.alpha, config.beta);
    
    ScheduleState bestState = currentState;
    
    double T = config.initialTemperature;
    
    RunResult result;
    result.runIndex = runIndex;

    int totalIterations = 0;
    while (T > config.minimumTemperature) {
        for (int i = 0; i < config.iterationsPerTemp; ++i) {
            ScheduleState nextState = currentState;
            generator.generate(dfg, nextState);
            CostEvaluator::evaluate(dfg, nextState, config.alpha, config.beta);
            
            double delta = nextState.cost - currentState.cost;
            
            if (delta < 0) {
                currentState = nextState;
            } else {
                double p = std::exp(-delta / T);
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                static std::mt19937 acceptanceGen(config.seed + runIndex + 1000); 
                if (dist(acceptanceGen) < p) {
                    currentState = nextState;
                }
            }
            
            if (currentState.cost < bestState.cost) {
                bestState = currentState;
            }
            
            totalIterations++;
            if (totalIterations % 100 == 0) {
                result.convergenceLog.push_back({totalIterations, bestState.cost});
                result.tempsLog.push_back(T);
            }
        }
        T *= config.coolingRate;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.bestState = bestState;
    
    return result;
}
