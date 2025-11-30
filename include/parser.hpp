#pragma once
#include <memory>
#include <vector>
#include <string>
#include "lexer.hpp"

// AST Node Types
enum class ASTNodeType {
    PROGRAM,
    TITLE_STMT,
    SAVE_STMT,
    SCREEN_STMT,
    TEXT_STMT,
    BUTTON_STMT,
    ROW_STMT,
    LOAD_STMT,
    PARAM_ASSIGNMENT
};

// Base AST Node
struct ASTNode {
    ASTNodeType type;
    size_t line;
    
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Program Node (root)
struct ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    ProgramNode() { type = ASTNodeType::PROGRAM; line = 0; }
    void print(int indent = 0) const override;
};

// Title Statement
struct TitleStmtNode : public ASTNode {
    std::string title;
    
    TitleStmtNode(const std::string& t, size_t l) : title(t) {
        type = ASTNodeType::TITLE_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Save Statement
struct SaveStmtNode : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    SaveStmtNode(const std::string& n, size_t l) : name(n) {
        type = ASTNodeType::SAVE_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Screen Statement
struct ScreenStmtNode : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    ScreenStmtNode(const std::string& n, size_t l) : name(n) {
        type = ASTNodeType::SCREEN_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Text Statement
struct TextStmtNode : public ASTNode {
    std::string text;
    
    TextStmtNode(const std::string& t, size_t l) : text(t) {
        type = ASTNodeType::TEXT_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Button Statement
struct ButtonStmtNode : public ASTNode {
    std::string label;
    
    ButtonStmtNode(const std::string& lbl, size_t l) : label(lbl) {
        type = ASTNodeType::BUTTON_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Row Statement
struct RowStmtNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> body;
    
    RowStmtNode(size_t l) {
        type = ASTNodeType::ROW_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Parameter Assignment
struct ParamAssignmentNode : public ASTNode {
    std::string name;
    std::string value;
    
    ParamAssignmentNode(const std::string& n, const std::string& v, size_t l) 
        : name(n), value(v) {
        type = ASTNodeType::PARAM_ASSIGNMENT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Load Statement
struct LoadStmtNode : public ASTNode {
    std::string componentName;
    std::string alias;
    bool hasAlias;
    std::vector<std::unique_ptr<ASTNode>> params;
    
    LoadStmtNode(const std::string& comp, size_t l) 
        : componentName(comp), hasAlias(false) {
        type = ASTNodeType::LOAD_STMT;
        line = l;
    }
    void print(int indent = 0) const override;
};

// Parser Class
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t pos = 0;

    Token peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& message);
    void skipNewlines();
    
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseTopLevelStmt();
    std::unique_ptr<TitleStmtNode> parseTitleStmt();
    std::unique_ptr<SaveStmtNode> parseSaveStmt();
    std::unique_ptr<ScreenStmtNode> parseScreenStmt();
    std::vector<std::unique_ptr<ASTNode>> parseBlock();
    std::unique_ptr<ASTNode> parseBlockStmt();
    std::unique_ptr<TextStmtNode> parseTextStmt();
    std::unique_ptr<ButtonStmtNode> parseButtonStmt();
    std::unique_ptr<RowStmtNode> parseRowStmt();
    std::unique_ptr<LoadStmtNode> parseLoadStmt();
    std::vector<std::unique_ptr<ASTNode>> parseLoadParamBlock();
    std::unique_ptr<ParamAssignmentNode> parseParamAssignment();
};
