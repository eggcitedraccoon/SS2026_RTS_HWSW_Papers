#ifndef SACONFIG_H
#define SACONFIG_H

struct SAConfig {
    double initialTemperature = 1000.0;
    double coolingRate = 0.95;
    double minimumTemperature = 0.01;
    int iterationsPerTemp = 100;
    double alpha = 1.0;
    double beta = 0.5;
    int runs = 30;
    int seed = 42;
};

#endif // SACONFIG_H
