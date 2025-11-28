#pragma once

#include <sstream>
#include <fstream>
#include <string>

inline std::string readFile(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
