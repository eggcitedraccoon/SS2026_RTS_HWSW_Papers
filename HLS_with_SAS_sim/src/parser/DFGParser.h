/**
 * @file DFGParser.h
 * @brief Provides a parser to convert a textual DFG description into the internal DFG model.
 */

#ifndef DFGPARSER_H
#define DFGPARSER_H

#include <string>
#include <map>
#include "../model/DFG.h"

/**
 * @class DFGParser
 * @brief Static class for parsing Data Flow Graph (DFG) files.
 */
class DFGParser {
public:
    /**
     * @brief Parses a DFG file and populates a DFG object.
     * @param filename Path to the DFG file.
     * @param dfg Reference to the DFG object to be populated.
     * @return true if parsing was successful, false otherwise.
     */
    static bool parseDFG(const std::string& filename, DFG& dfg);
};

#endif // DFGPARSER_H
