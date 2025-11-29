#pragma once
#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    STRING,
    INDENT,
    NEWLINE,
    AT_SYMBOL,
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
