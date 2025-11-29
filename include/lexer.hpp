#pragma once
#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    AT_SYMBOL,
    IDENTIFIER,
    COLON,
    STRING,
    NUMBER,
    COMMA,
    LBRACKET,
    RBRACKET,
    NEWLINE,
    INDENT,
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
    std::string source;
    size_t pos = 0;
    size_t line = 1;
};
