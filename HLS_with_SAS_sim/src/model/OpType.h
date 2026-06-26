/**
 * @file OpType.h
 * @brief Defines the supported operation types and utility functions for conversion.
 */

#ifndef OPTYPE_H
#define OPTYPE_H

#include <string>
#include <map>

/**
 * @enum OpType
 * @brief Enumeration of supported operations in the DFG.
 */
enum class OpType {
    LOAD,    ///< Memory load operation
    STORE,   ///< Memory store operation
    ADD,     ///< Addition operation
    SUB,     ///< Subtraction operation
    MUL,     ///< Multiplication operation
    DIV,     ///< Division operation
    UNKNOWN  ///< Fallback for unknown operations
};

/**
 * @brief Converts a string representation of an operation type to an OpType enum.
 * @param type The string representation (e.g., "ADD", "MUL").
 * @return The corresponding OpType enum, or OpType::UNKNOWN if not found.
 */
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

/**
 * @brief Converts an OpType enum to its string representation.
 * @param type The OpType enum.
 * @return The string representation of the operation type.
 */
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
