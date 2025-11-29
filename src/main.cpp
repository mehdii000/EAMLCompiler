#include <iostream>
#include <vector>

#include "codeutils.hpp"
#include "lexer.hpp"

std::string getType(TokenType type) {
    switch (type) {
        case TokenType::AT_SYMBOL:
            return "AT_SYMBOL";
            break;
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
            break;
        case TokenType::COLON:
            return "COLON";
            break;
        case TokenType::STRING:
            return "STRING";
            break;
        case TokenType::NUMBER:
            return "NUMBER";
            break;
        case TokenType::COMMA:
            return "COMMA";
            break;
        case TokenType::LBRACKET:
            return "LBRACKET";
            break;
        case TokenType::RBRACKET:
            return "RBRACKET";
            break;
        case TokenType::NEWLINE:
            return "NEWLINE";
            break;
        case TokenType::INDENT:
            return "INDENT";
            break;
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

int main(int argc, char const *argv[])
{
    std::string source = readFile(argv[1]);
    
    std::cout << "====== LEXING =======" << std::endl;
    std::cout << source;
    std::cout << "=====================" << std::endl;

    std::vector<Token> tokens;
    Lexer lexer(source);
    tokens = lexer.tokenize();
    
    for (Token token : tokens) {
        std::cout << getType(token.type);
        if (token.value.has_value()) {
            std::cout << " -> " << token.value.value() << std::endl;
        } else {
            std::cout << std::endl;
        }
    }

    return 0;
}
