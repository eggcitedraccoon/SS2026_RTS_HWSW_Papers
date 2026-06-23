#include "DFGParser.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

bool DFGParser::parseDFG(const std::string& filename, DFG& dfg) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Could not open DFG file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int idCounter = 0;
    std::map<std::string, int> varToId;

    // Regex for: t1 = OP(arg1, arg2) or t1 = OP(arg1)
    std::regex assignOpRegex(R"(^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*$)");
    // Regex for: OP(arg1)
    std::regex loneOpRegex(R"(^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*$)");

    int edgeCount = 0;
    while (std::getline(f, line)) {
        // Remove comments
        auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        // Trim line
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1, std::string::npos);
        
        if (line.empty()) continue;

        std::smatch match;
        if (std::regex_match(line, match, assignOpRegex)) {
            std::string varName = match[1];
            std::string opName = match[2];
            std::string argsStr = match[3];

            OpType type = stringToOpType(opName);
            int currentId = idCounter++;
            varToId[varName] = currentId;

            auto op = std::make_unique<Operation>(currentId, varName, type);
            dfg.addOperation(std::move(op));
            
            // Parse arguments
            std::stringstream ss(argsStr);
            std::string arg;
            while (std::getline(ss, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                if (varToId.count(arg)) {
                    dfg.addEdge(varToId[arg], currentId);
                    edgeCount++;
                }
            }

        } else if (std::regex_match(line, match, loneOpRegex)) {
            std::string opName = match[1];
            std::string argsStr = match[2];

            OpType type = stringToOpType(opName);
            int currentId = idCounter++;
            // Lone ops might not have a return variable (like STORE)
            auto op = std::make_unique<Operation>(currentId, "op_" + std::to_string(currentId), type);
            dfg.addOperation(std::move(op));

            std::stringstream ss(argsStr);
            std::string arg;
            while (std::getline(ss, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                if (varToId.count(arg)) {
                    dfg.addEdge(varToId[arg], currentId);
                    edgeCount++;
                }
            }
        } else {
            std::cerr << "Warning: Could not parse line: " << line << std::endl;
        }
    }
    return true;
}
