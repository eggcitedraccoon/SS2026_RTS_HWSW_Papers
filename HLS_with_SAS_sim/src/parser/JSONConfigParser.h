/**
 * @file JSONConfigParser.h
 * @brief Provides a parser for JSON configuration files containing SA parameters and resource pools.
 */

#ifndef JSONCONFIGPARSER_H
#define JSONCONFIGPARSER_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../model/SAConfig.h"
#include "../model/DFG.h"
#include "../model/ScheduleState.h"

/**
 * @class JSONConfigParser
 * @brief Static class for parsing JSON configuration files.
 */
class JSONConfigParser {
public:
    /**
     * @brief Parses a JSON config file and initializes SA configuration and initial resource pool.
     * @param filename Path to the JSON config file.
     * @param config Reference to SAConfig to be populated.
     * @param dfg Reference to DFG to store operation type info.
     * @param initialState Reference to ScheduleState to store the initial resource pool.
     * @return true if parsing was successful, false otherwise.
     */
    static bool parseConfig(const std::string& filename, SAConfig& config, DFG& dfg, ScheduleState& initialState);
};

#endif // JSONCONFIGPARSER_H
