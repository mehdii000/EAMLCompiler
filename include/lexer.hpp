#pragma once
#include <string>
#include <vector>

enum class TokenType {
    AT_SYMBOL,
    IDENTIFIER,
    STRING,
    COLON,
    LBRACKET,
    RBRACKET,
    NEWLINE,
    INDENT,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};
  
class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos = 0;
    int line = 1;
    int col = 1;
};
