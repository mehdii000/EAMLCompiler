#include "parser.hpp"
#include <iostream>
#include <stdexcept>

static void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }
}

static void printTree(const ASTNode* node, const std::string& prefix = "", bool isLast = true) {
    if (!node) return;
    
    std::cout << prefix;
    std::cout << (isLast ? "`--> " : "|-> ");
    
    if (auto* root = dynamic_cast<const RootNode*>(node)) {
        std::cout << "[Program]" << std::endl;
        for (size_t i = 0; i < root->statements.size(); i++) {
            printTree(root->statements[i].get(), 
                     prefix + (isLast ? "    " : "|   "),
                     i == root->statements.size() - 1);
        }
    }
    else if (auto* title = dynamic_cast<const TitleStmtNode*>(node)) {
        std::cout << "@title \"" << title->title << "\"" << std::endl;
    }
    else if (auto* screen = dynamic_cast<const ScreenStmtNode*>(node)) {
        std::cout << "@screen " << screen->name << std::endl;
        for (size_t i = 0; i < screen->body.size(); i++) {
            printTree(screen->body[i].get(),
                     prefix + (isLast ? "    " : "|   "),
                     i == screen->body.size() - 1);
        }
    }
    else if (auto* text = dynamic_cast<const TextStmtNode*>(node)) {
        std::cout << "@text \"" << text->text << "\"" << std::endl;
    }
    else if (auto* save = dynamic_cast<const SaveStmtNode*>(node)) {
        std::cout << "@save " << save->name << std::endl;
        for (size_t i = 0; i < save->body.size(); i++) {
            printTree(save->body[i].get(),
                     prefix + (isLast ? "    " : "|   "),
                     i == save->body.size() - 1);
        }
    }
    else if (auto* load = dynamic_cast<const LoadStmtNode*>(node)) {
        std::cout << "@load " << load->name << std::endl;
        for (size_t i = 0; i < load->parameters.size(); i++) {
            printTree(load->parameters[i].get(),
                     prefix + (isLast ? "    " : "|   "),
                     i == load->parameters.size() - 1);
        }
    } else if (auto* generic = dynamic_cast<const GenericAtStmtNode*>(node)) {
        std::cout << "@" << generic->name << " ";
        for (const auto& [k, v] : generic->htmlData) {
            std::cout << k << "=" << v << " ";
        }
        std::cout << std::endl;
    }
    else if (auto* param = dynamic_cast<const ParameterNode*>(node)) {
        std::cout << param->name << ": " << param->value << std::endl;
    }
}

void printPrettyTree(const RootNode* root) {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "          AST Tree View" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n";
    printTree(root, "", true);
    std::cout << "\n";
}

void RootNode::print(int indent) const {
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

void ParameterNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Parameter: " << name << " = " << value << std::endl;
}

void SaveStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "SaveStmt: " << name << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
}

void LoadStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "LoadStmt: " << name << std::endl;
    for (const auto& param : parameters) {
        param->print(indent + 1);
    }
}

void GenericAtStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "GenericAtStmt: " << name << " ";
    for (const auto& [k, v] : htmlData) {
        std::cout << k << "=" << v << " ";
    }
    std::cout << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
    std::cout << std::endl;
}

void LayoutStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "LayoutStmt: " << layout << std::endl;
    for (const auto& stmt : body) {
        stmt->print(indent + 1);
    }
}


Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek(int offset) const {
    if (pos + offset < tokens.size()) {
        return tokens[pos + offset];
    }
    return tokens.back();
}

Token Parser::consume() {
    if (pos < tokens.size()) {
        return tokens[pos++];
    }
    return tokens.back();
}

void Parser::skipNewlines() {
    while (peek().type == TokenType::NEWLINE) {
        consume();
    }
}

std::unique_ptr<RootNode> Parser::parseProgram() {
    auto program = std::make_unique<RootNode>();
    
    skipNewlines();
    
    while (peek().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement(0);
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        skipNewlines();
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement(int currentIndent) {
    int indent = 0;
    while (peek(indent).type == TokenType::INDENT) {
        indent++;
    }
    
    if (indent != currentIndent) {
        return nullptr;
    }
    
    for (int i = 0; i < indent; i++) {
        consume();
    }
    
    switch (peek().type) {
        case TokenType::AT_TITLE:
            return parseTitleStmt();
        case TokenType::AT_SCREEN:
            return parseScreenStmt(currentIndent);
        case TokenType::AT_TEXT:
            return parseTextStmt();
        case TokenType::AT_SAVE:
            return parseSaveStmt(currentIndent);
        case TokenType::AT_LOAD:
            return parseLoadStmt(currentIndent);
        case TokenType::AT_IDENTIFIER:
            return parseGenericAtStmt(currentIndent);
        case TokenType::AT_ROW:
        case TokenType::AT_STACK:
        case TokenType::AT_LEFT:
        case TokenType::AT_RIGHT:
        case TokenType::AT_CENTER:
            return parseLayoutStmt(currentIndent, peek().type);
        default:
            return nullptr;
    }
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock(int parentIndent) {
    std::vector<std::unique_ptr<ASTNode>> statements;
    int blockIndent = parentIndent + 1;
    
    skipNewlines();
    
    while (peek().type != TokenType::END_OF_FILE) {

        // Count indentation
        int indent = 0;
        int offset = 0;
        while (peek(offset).type == TokenType::INDENT) {
            indent++;
            offset++;
        }

        // Case 1: blank line → skip it
        if (peek(offset).type == TokenType::NEWLINE) {
            // consume the entire blank line
            for (int i = 0; i < offset; i++)
                consume();
            // consume NEWLINE
            consume();
            continue;        // do not treat as a statement
        }

        // Case 2: indentation mismatch
        if (indent < blockIndent)
            break;

        if (indent > blockIndent)
            throw std::runtime_error("Unexpected indentation at line " +
                                    std::to_string(peek().line));

        // Parse normally
        auto stmt = parseStatement(blockIndent);
        if (stmt) statements.push_back(std::move(stmt));
        else break;

        skipNewlines();
    }
    
    return statements;
}

std::unique_ptr<TitleStmtNode> Parser::parseTitleStmt() {
    consume(); // consume @title
    
    if (peek().type != TokenType::STRING) {
        throw std::runtime_error("Expected string after @title at line " + 
                               std::to_string(peek().line));
    }
    
    std::string title = consume().value.value();
    
    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after title at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume newline
    
    return std::make_unique<TitleStmtNode>(title);
}

std::unique_ptr<ScreenStmtNode> Parser::parseScreenStmt(int currentIndent) {
    consume(); // consume @screen
    
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after @screen at line " + 
                               std::to_string(peek().line));
    }
    
    std::string screenName = consume().value.value();
    auto screen = std::make_unique<ScreenStmtNode>(screenName);

    if (peek().type != TokenType::COLON) {
        throw std::runtime_error("Expected colon after screen name at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume colon
    
    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after colon at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume newline before parsing block
    
    screen->body = parseBlock(currentIndent);
    
    return screen;
}

std::unique_ptr<TextStmtNode> Parser::parseTextStmt() {
    consume(); // consume @text
    
    if (peek().type != TokenType::STRING) {
        throw std::runtime_error("Expected string after @text at line " + 
                               std::to_string(peek().line));
    }
    
    std::string text = consume().value.value();
    
    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after text string at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume newline
    
    return std::make_unique<TextStmtNode>(text);
}

std::unique_ptr<SaveStmtNode> Parser::parseSaveStmt(int currentIndent) {
    consume(); // consume @save
    
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after @save at line " + 
                               std::to_string(peek().line));
    }
    
    std::string componentName = consume().value.value();
    auto component = std::make_unique<SaveStmtNode>(componentName);

    if (peek().type != TokenType::COLON) {
        throw std::runtime_error("Expected colon after component name at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume colon
    
    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after colon at line " + 
                               std::to_string(peek().line));
    }
    consume(); // consume newline
    
    component->body = parseBlock(currentIndent);
    
    return component;
}

std::unique_ptr<LoadStmtNode> Parser::parseLoadStmt(int currentIndent) {
    consume(); // consume @load
    
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after @load at line " + 
                               std::to_string(peek().line));
    }
    
    std::string componentName = consume().value.value();
    auto component = std::make_unique<LoadStmtNode>(componentName);

    if (peek().type == TokenType::WITH) {
        consume(); // consume WITH

        if (peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected colon after 'with' at line " + 
                                   std::to_string(peek().line));
        }
        consume(); // consume colon
        
        if (peek().type != TokenType::NEWLINE) {
            throw std::runtime_error("Expected newline after colon at line " + 
                                   std::to_string(peek().line));
        }
        consume(); // consume newline

        component->parameters = std::move(parseParameters(currentIndent));

    } else {
        if (peek().type != TokenType::NEWLINE) {
            throw std::runtime_error("Expected newline after load statement at line " + 
                                   std::to_string(peek().line));
        }
        component->parameters = std::vector<std::unique_ptr<ParameterNode>>();
        consume(); // consume newline
    }

    return component;
}

std::unique_ptr<GenericAtStmtNode> Parser::parseGenericAtStmt(int currentIndent) {
    std::string genericName = consume().value.value(); // consume and return @<value>

    std::string headerValue = peek().type == TokenType::STRING ? consume().value.value() : "";
    std::vector<std::pair<std::string, std::string>> htmlParams;

    while (peek().type == TokenType::IDENTIFIER
           && peek(1).type == TokenType::EQUAL
           && peek(2).type == TokenType::STRING) {
            std::string htmlParam = consume().value.value();
            consume(); // consume =
            std::string htmlValue = consume().value.value();
            htmlParams.push_back(std::make_pair(htmlParam, htmlValue));
    }

    std::vector<std::unique_ptr<ASTNode>> body;
    if (peek().type == TokenType::COLON) {
        consume(); // consume colon
        body = parseBlock(currentIndent);
    } else {
        body = std::vector<std::unique_ptr<ASTNode>>();
    }

    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after generic at statement at line " + 
            std::to_string(peek().line));
    }
    consume(); // consume newline
    
    auto generic = std::make_unique<GenericAtStmtNode>(genericName, headerValue);
    generic->htmlData = std::move(htmlParams);
    generic->body = std::move(body);
    return generic;

}

std::vector<std::unique_ptr<ParameterNode>> Parser::parseParameters(int parentIndent) {
    std::vector<std::unique_ptr<ParameterNode>> parameters;
    int blockIndent = parentIndent + 1;
    
    skipNewlines();
    
    while (peek().type != TokenType::END_OF_FILE) {
        int indent = 0;
        int offset = 0;
        while (peek(offset).type == TokenType::INDENT) {
            indent++;
            offset++;
        }
        
        if (indent < blockIndent) {
            break;
        }
        
        if (indent > blockIndent) {
            throw std::runtime_error("Unexpected indentation at line " + 
                                   std::to_string(peek().line));
        }
        
        for (int i = 0; i < indent; i++) {
            consume();
        }
        
        if (peek().type != TokenType::IDENTIFIER) {
            break;
        }
        
        std::string paramName = consume().value.value();
        
        if (peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected colon after parameter name at line " + 
                                   std::to_string(peek().line));
        }
        consume(); // consume colon
        
        if (peek().type != TokenType::STRING && peek().type != TokenType::IDENTIFIER && peek().type != TokenType::NUMBER) {
            throw std::runtime_error("Expected value after colon at line " + 
                                   std::to_string(peek().line));
        }
        
        std::string paramValue = consume().value.value();
        parameters.push_back(std::make_unique<ParameterNode>(paramName, paramValue));
        
        skipNewlines();
    }
    
    return parameters;
}

std::unique_ptr<LayoutStmtNode> Parser::parseLayoutStmt(int currentIndent, TokenType type) {
    std::string layout = "";

    if (peek().type != type) {
        throw std::runtime_error("Expected layout type at line " +
                               std::to_string(peek().line));
    }

    switch (type) {
        case TokenType::AT_ROW:
            layout = "row";
            break;
        case TokenType::AT_STACK:
            layout = "stack";
            break;        case TokenType::AT_LEFT:
            layout = "left";
            break;
        case TokenType::AT_RIGHT:
            layout = "right";
            break;
        case TokenType::AT_CENTER:
            layout = "center";
            break;
        default:
            throw std::runtime_error("Unknown layout type");
    }
    consume(); // consume layout type token

    auto layoutNode = std::make_unique<LayoutStmtNode>();
    layoutNode->layout = layout;

    if (peek().type != TokenType::COLON) {
        throw std::runtime_error("Expected colon after layout type at line " +
                               std::to_string(peek().line));
    }
    consume(); // consume colon

    if (peek().type != TokenType::NEWLINE) {
        throw std::runtime_error("Expected newline after colon at line " +
                               std::to_string(peek().line));
    }
    consume(); // consume newline

    layoutNode->body = parseBlock(currentIndent);

    return layoutNode;
}
