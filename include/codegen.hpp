#pragma once
#include "parser.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

class CodeGenerator {
private:
    std::unordered_map<std::string, std::vector<std::unique_ptr<ASTNode>>> atSaveTable;

    std::unique_ptr<ASTNode> cloneNode(const ASTNode* node);
    void expandLoadsInList(std::vector<std::unique_ptr<ASTNode>>& list);
    std::string generateHTMLOutput(RootNode& root);


public:
    void generate(RootNode& root);
};
