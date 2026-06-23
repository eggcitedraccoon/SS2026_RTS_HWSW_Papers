#ifndef DFGPARSER_H
#define DFGPARSER_H

#include <string>
#include <map>
#include "../model/DFG.h"

class DFGParser {
public:
    static bool parseDFG(const std::string& filename, DFG& dfg);
};

#endif // DFGPARSER_H
