#ifndef COSTEVALUATOR_H
#define COSTEVALUATOR_H

#include "../model/DFG.h"
#include "../model/ScheduleState.h"
#include "../model/SAConfig.h"

class CostEvaluator {
public:
    static double evaluate(const DFG& dfg, ScheduleState& state, double alpha, double beta);
};

#endif // COSTEVALUATOR_H
