#include "parser.hpp"
#include <iostream>
#include <stdexcept>

// Helper function to print indentation
static void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }
}

// Print implementations
void ProgramNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Program" << std::endl;
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

void TitleStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "TitleStmt: \"" << title << "\"" << std::endl;
}

void SaveStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "SaveStmt: " << name << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
}

void ScreenStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ScreenStmt: " << name << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
}

void TextStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "TextStmt: \"" << text << "\"" << std::endl;
}

void ButtonStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ButtonStmt: \"" << label << "\"" << std::endl;
}

void RowStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "RowStmt" << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
}

void ParamAssignmentNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Param: " << name << " = \"" << value << "\"" << std::endl;
}

void LoadStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "LoadStmt: " << componentName;
    if (hasAlias) {
        std::cout << " (alias: " << alias << ")";
    }
    std::cout << std::endl;
    for (const auto& param : params) {
        param->print(indent + 1);
    }
}

// Parser implementation
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek() const {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return tokens.back(); // Return END_OF_FILE
}

Token Parser::advance() {
    if (pos < tokens.size()) {
        return tokens[pos++];
    }
    return tokens.back();
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& message) {
    Token tok = peek();
    if (tok.type != type) {
        throw std::runtime_error("Parse error Multi-step fluid processing at line " + std::to_string(tok.line) + ": " + message);
    }
    return advance();
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE)) {}
}

std::unique_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    
    // Skip leading newlines
    skipNewlines();
    
    // Parse top-level statements
    while (peek().type != TokenType::END_OF_FILE) {
        auto stmt = parseTopLevelStmt();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        skipNewlines();
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseTopLevelStmt() {
    Token tok = peek();
    
    switch (tok.type) {
        case TokenType::AT_TITLE:
            return parseTitleStmt();
        case TokenType::AT_SAVE:
            return parseSaveStmt();
        case TokenType::AT_SCREEN:
            return parseScreenStmt();
        default:
            throw std::runtime_error("Unexpected token at line " + std::to_string(tok.line));
    }
}

std::unique_ptr<TitleStmtNode> Parser::parseTitleStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_TITLE, "Expected @title");
    Token str = expect(TokenType::STRING, "Expected string after @title");
    expect(TokenType::NEWLINE, "Expected newline after title string");
    
    return std::make_unique<TitleStmtNode>(str.value.value(), line);
}

std::unique_ptr<SaveStmtNode> Parser::parseSaveStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_SAVE, "Expected @save");
    Token id = expect(TokenType::IDENTIFIER, "Expected identifier after @save");
    expect(TokenType::COLON, "Expected ':' after save identifier");
    expect(TokenType::NEWLINE, "Expected newline after ':'");
    
    auto saveNode = std::make_unique<SaveStmtNode>(id.value.value(), line);
    saveNode->body = parseBlock();
    
    return saveNode;
}

std::unique_ptr<ScreenStmtNode> Parser::parseScreenStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_SCREEN, "Expected @screen");
    Token id = expect(TokenType::IDENTIFIER, "Expected identifier after @screen");
    expect(TokenType::COLON, "Expected ':' after screen identifier");
    expect(TokenType::NEWLINE, "Expected newline after ':'");
    
    auto screenNode = std::make_unique<ScreenStmtNode>(id.value.value(), line);
    screenNode->body = parseBlock();
    
    return screenNode;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (peek().type == TokenType::INDENT) {
        advance(); // consume INDENT
        auto stmt = parseBlockStmt();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseBlockStmt() {
    Token tok = peek();
    
    switch (tok.type) {
        case TokenType::AT_TEXT:
            return parseTextStmt();
        case TokenType::AT_IDENTIFIER:
            return parseButtonStmt();
        case TokenType::AT_ROW:
            return parseRowStmt();
        case TokenType::AT_LOAD:
            return parseLoadStmt();
        case TokenType::NEWLINE:
            advance();
            return nullptr;
        default:
            throw std::runtime_error("Unexpected token in block at line " + std::to_string(tok.line));
    }
}

std::unique_ptr<TextStmtNode> Parser::parseTextStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_TEXT, "Expected @text");
    Token str = expect(TokenType::STRING, "Expected string after @text");
    expect(TokenType::NEWLINE, "Expected newline after text string");
    
    return std::make_unique<TextStmtNode>(str.value.value(), line);
}

std::unique_ptr<ButtonStmtNode> Parser::parseButtonStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_IDENTIFIER, "Expected @identifier");
    Token str = expect(TokenType::STRING, "Expected string after @identifier");
    expect(TokenType::NEWLINE, "Expected newline after button string");
    
    return std::make_unique<ButtonStmtNode>(str.value.value(), line);
}

std::unique_ptr<RowStmtNode> Parser::parseRowStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_ROW, "Expected @row");
    expect(TokenType::COLON, "Expected ':' after @row");
    expect(TokenType::NEWLINE, "Expected newline after ':'");
    
    auto rowNode = std::make_unique<RowStmtNode>(line);
    rowNode->body = parseBlock();
    
    return rowNode;
}

std::unique_ptr<LoadStmtNode> Parser::parseLoadStmt() {
    size_t line = peek().line;
    expect(TokenType::AT_LOAD, "Expected @load");
    Token component = expect(TokenType::IDENTIFIER, "Expected component name after @load");
    
    auto loadNode = std::make_unique<LoadStmtNode>(component.value.value(), line);
    
    // Check for optional alias (IDENTIFIER)
    if (peek().type == TokenType::IDENTIFIER) {
        Token aliasToken = advance();
        loadNode->alias = aliasToken.value.value();
        loadNode->hasAlias = true;
        
        // The alias should be "with", but we store it anyway
        // You can add validation here if needed
    }
    
    expect(TokenType::COLON, "Expected ':' after @load");
    expect(TokenType::NEWLINE, "Expected newline after ':'");
    
    loadNode->params = parseLoadParamBlock();
    
    return loadNode;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseLoadParamBlock() {
    std::vector<std::unique_ptr<ASTNode>> params;
    
    while (peek().type == TokenType::INDENT) {
        advance(); // consume INDENT
        auto param = parseParamAssignment();
        if (param) {
            params.push_back(std::move(param));
        }
    }
    
    return params;
}

std::unique_ptr<ParamAssignmentNode> Parser::parseParamAssignment() {
    size_t line = peek().line;
    Token name = expect(TokenType::IDENTIFIER, "Expected parameter name");
    expect(TokenType::COLON, "Expected ':' after parameter name");
    Token value = expect(TokenType::STRING, "Expected string value for parameter");
    expect(TokenType::NEWLINE, "Expected newline after parameter value");
    
    return std::make_unique<ParamAssignmentNode>(name.value.value(), value.value.value(), line);
}
