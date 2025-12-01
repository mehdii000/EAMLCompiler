#pragma once
#include "lexer.hpp"
#include <memory>
#include <vector>
#include <string>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;

    // Return pointer to an owned children vector if this node type contains ASTNode children.
    // Default: no children.
    virtual std::vector<std::unique_ptr<ASTNode>>* children() { return nullptr; }
};

struct RootNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    void print(int indent = 0) const override;
    std::vector<std::unique_ptr<ASTNode>>* children() override { return &statements; }
};

struct TitleStmtNode : ASTNode {
    std::string title;
    TitleStmtNode(const std::string& t) : title(t) {}
    void print(int indent = 0) const override;
};

struct ScreenStmtNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    ScreenStmtNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override;
    std::vector<std::unique_ptr<ASTNode>>* children() override { return &body; }
};

struct TextStmtNode : ASTNode {
    std::string text;
    TextStmtNode(const std::string& t) : text(t) {}
    void print(int indent = 0) const override;
};

struct ParameterNode : ASTNode {
    std::string name;
    std::string value;
    ParameterNode(const std::string& n, const std::string& v) : name(n), value(v) {}
    void print(int indent = 0) const override;
    // ParameterNode is a leaf; no children() override.
};

struct SaveStmtNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    SaveStmtNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override;
    std::vector<std::unique_ptr<ASTNode>>* children() override { return &body; }
};

struct LoadStmtNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ParameterNode>> parameters;
    LoadStmtNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override;
    // LoadStmtNode has parameters (ParameterNode) but not ASTNode-body children
    // so we don't expose children() for structural AST replacement purposes.
};

struct GenericAtStmtNode : ASTNode {
    std::string name;
    std::string value;
    GenericAtStmtNode(const std::string& n, const std::string& v = "") : name(n), value(v) {}
    void print(int indent = 0) const override;
};

void printPrettyTree(const RootNode* root);

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek(int offset = 0) const;
    Token consume();
    void skipNewlines();
    
    std::unique_ptr<ASTNode> parseStatement(int currentIndent);
    std::vector<std::unique_ptr<ASTNode>> parseBlock(int parentIndent);
    std::vector<std::unique_ptr<ParameterNode>> parseParameters(int parentIndent);
    
    std::unique_ptr<TitleStmtNode> parseTitleStmt();
    std::unique_ptr<ScreenStmtNode> parseScreenStmt(int currentIndent);
    std::unique_ptr<TextStmtNode> parseTextStmt();
    std::unique_ptr<SaveStmtNode> parseSaveStmt(int currentIndent);
    std::unique_ptr<LoadStmtNode> parseLoadStmt(int currentIndent);
    std::unique_ptr<GenericAtStmtNode> parseGenericAtStmt(int currentIndent);

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<RootNode> parseProgram();
};
