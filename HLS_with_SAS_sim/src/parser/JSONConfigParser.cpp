/**
 * @file JSONConfigParser.cpp
 * @brief Implementation of the JSONConfigParser class.
 */

#include "JSONConfigParser.h"
#include <iostream>

using json = nlohmann::json;

/**
 * @brief Parses the JSON configuration and sets up the initial environment.
 * 
 * Extracts SA parameters (alpha, beta, cooling rate, etc.) and populates the 
 * initial resource pool and operation type metadata (latency, area).
 */
bool JSONConfigParser::parseConfig(const std::string& filename, SAConfig& config, DFG& dfg, ScheduleState& initialState) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return false;
    }

    try {
        json data = json::parse(f);

        if (data.contains("sa")) {
            auto& sa = data["sa"];
            if (sa.contains("initial_temperature")) config.initialTemperature = sa["initial_temperature"];
            if (sa.contains("cooling_rate")) config.coolingRate = sa["cooling_rate"];
            if (sa.contains("min_temperature")) config.minimumTemperature = sa["min_temperature"];
            if (sa.contains("iterations_per_temp")) config.iterationsPerTemp = sa["iterations_per_temp"];
            if (sa.contains("alpha")) config.alpha = sa["alpha"];
            if (sa.contains("beta")) config.beta = sa["beta"];
            if (sa.contains("runs")) config.runs = sa["runs"];
            if (sa.contains("seed")) config.seed = sa["seed"];
        }

        int resourceIdCounter = 0;
        if (data.contains("resources")) {
            for (auto& [name, info] : data["resources"].items()) {
                OpType type = stringToOpType(name);
                int count = info.value("count", 1);
                int area = info.value("area", 1);
                int latency = info.value("latency", 1);

                dfg.opTypeInfo[type] = {latency, area};

                for (int i = 0; i < count; ++i) {
                    initialState.resources.emplace(resourceIdCounter, Resource(resourceIdCounter, type, area));
                    resourceIdCounter++;
                }
            }
        } else {
            // Default minimum set
            std::map<std::string, std::pair<int, int>> defaults = {
                {"LOAD", {1, 1}}, {"STORE", {1, 1}}, {"ADD", {1, 1}}, {"SUB", {1, 1}}, {"MUL", {2, 3}}, {"DIV", {4, 8}}
            };
            for (auto const& [name, params] : defaults) {
                OpType type = stringToOpType(name);
                dfg.opTypeInfo[type] = {params.first, params.second};
                initialState.resources.emplace(resourceIdCounter, Resource(resourceIdCounter, type, params.second));
                resourceIdCounter++;
            }
        }

        return true;
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}
