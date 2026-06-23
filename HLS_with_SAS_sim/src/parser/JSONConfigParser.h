#ifndef JSONCONFIGPARSER_H
#define JSONCONFIGPARSER_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../model/SAConfig.h"
#include "../model/DFG.h"
#include "../model/ScheduleState.h"

class JSONConfigParser {
public:
    static bool parseConfig(const std::string& filename, SAConfig& config, DFG& dfg, ScheduleState& initialState);
};

#endif // JSONCONFIGPARSER_H
