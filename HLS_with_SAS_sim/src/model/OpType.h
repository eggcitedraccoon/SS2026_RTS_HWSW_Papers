#ifndef OPTYPE_H
#define OPTYPE_H

#include <string>
#include <map>

enum class OpType {
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    UNKNOWN
};

inline OpType stringToOpType(const std::string& type) {
    static const std::map<std::string, OpType> typeMap = {
        {"LOAD", OpType::LOAD},
        {"STORE", OpType::STORE},
        {"ADD", OpType::ADD},
        {"SUB", OpType::SUB},
        {"MUL", OpType::MUL},
        {"DIV", OpType::DIV}
    };
    auto it = typeMap.find(type);
    if (it != typeMap.end()) return it->second;
    return OpType::UNKNOWN;
}

inline std::string opTypeToString(OpType type) {
    static const std::map<OpType, std::string> typeMap = {
        {OpType::LOAD, "LOAD"},
        {OpType::STORE, "STORE"},
        {OpType::ADD, "ADD"},
        {OpType::SUB, "SUB"},
        {OpType::MUL, "MUL"},
        {OpType::DIV, "DIV"},
        {OpType::UNKNOWN, "UNKNOWN"}
    };
    return typeMap.at(type);
}

#endif // OPTYPE_H
