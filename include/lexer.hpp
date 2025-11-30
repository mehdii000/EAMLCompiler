#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

enum class TokenType {

    AT_KEYWORD,
    AT_TITLE,
    AT_CONSTANT,
    AT_SAVE,
    AT_SCREEN,
    AT_LOAD,
    AT_ROW,
    AT_STACK,
    AT_TEXT,

    AT_IDENTIFIER,

    IDENTIFIER,

    NUMBER,
    STRING,

    INDENT,
    NEWLINE,

    COLON,
    COMMA,
    LBRACKET,
    RBRACKET,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
    size_t line;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    const std::string source;
    size_t pos = 0;
    size_t line = 1;
};

static const std::unordered_map<std::string, TokenType> BUILTIN_TOKENS = {
    {"title", TokenType::AT_TITLE},
    {"const", TokenType::AT_CONSTANT},
    {"save", TokenType::AT_SAVE},
    {"screen", TokenType::AT_SCREEN},
    {"load", TokenType::AT_LOAD},
    {"row", TokenType::AT_ROW},
    {"stack", TokenType::AT_STACK},
    {"text", TokenType::AT_TEXT}
};
