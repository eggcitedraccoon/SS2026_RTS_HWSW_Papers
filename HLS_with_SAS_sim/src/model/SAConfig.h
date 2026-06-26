/**
 * @file SAConfig.h
 * @brief Configuration parameters for the Simulated Annealing algorithm.
 */

#ifndef SACONFIG_H
#define SACONFIG_H

/**
 * @struct SAConfig
 * @brief Parameters that control the behavior of the Simulated Annealing scheduler.
 */
struct SAConfig {
    double initialTemperature = 1000.0; ///< Starting temperature for SA
    double coolingRate = 0.95;          ///< Rate at which temperature decreases (T = T * coolingRate)
    double minimumTemperature = 0.01;   ///< Stopping temperature
    int iterationsPerTemp = 100;        ///< Number of neighbor moves to try at each temperature step
    double alpha = 1.0;                 ///< Weight for latency in the cost function
    double beta = 0.5;                  ///< Weight for area in the cost function
    double latencyNorm = 1.0;           ///< Normalization factor for latency
    double areaNorm = 1.0;              ///< Normalization factor for area
    int runs = 30;                      ///< Number of independent SA runs for statistical analysis
    int seed = 42;                      ///< Random seed for reproducibility
};

#endif // SACONFIG_H
