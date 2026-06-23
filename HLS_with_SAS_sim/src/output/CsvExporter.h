#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <string>
#include <vector>
#include "../scheduler/SimulatedAnnealingScheduler.h"

class CsvExporter {
public:
    static void exportRunSummary(const std::string& filename, const std::vector<RunResult>& results);
    static void exportConvergenceLog(const std::string& filename, const RunResult& result, const std::vector<double>& temps);
    static void exportGantt(const std::string& filename, const DFG& dfg, const ScheduleState& state);
};

#endif // CSVEXPORTER_H
