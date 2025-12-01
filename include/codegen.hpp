#pragma once
#include "parser.hpp"

class CodeGenerator {

public:
    CodeGenerator() = default;
    void generate(const RootNode* root);

};
