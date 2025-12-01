#include <iostream>
#include <vector>

#include "codeutils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "anaylzer.hpp"
#include "codegen.hpp"

std::string getType(TokenType type) {
    switch (type) {
        case TokenType::AT_TITLE: return "AT_TITLE";
        case TokenType::AT_CONSTANT: return "AT_CONSTANT";
        case TokenType::AT_SAVE: return "AT_SAVE";
        case TokenType::AT_SCREEN: return "AT_SCREEN";
        case TokenType::AT_LOAD: return "AT_LOAD";
        case TokenType::AT_ROW: return "AT_ROW";
        case TokenType::AT_STACK: return "AT_STACK";
        case TokenType::AT_TEXT: return "AT_TEXT";
        case TokenType::AT_IDENTIFIER: return "AT_IDENTIFIER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::INDENT: return "INDENT";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
    return "UNKNOWN";
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    std::string source = readFile(argv[1]);

    // Lexing
    std::cout << "Tokenizing..." << std::endl;

    std::vector<Token> tokens;
    Lexer lexer(source);
    tokens = lexer.tokenize();

    /*for (const Token& token : tokens) {
        std::string token_type_str = getType(token.type);
        
        if (token_type_str == "NEWLINE") {
            std::cout << " #\n";
        } 
        else if (token_type_str == "INDENT") {
            std::cout << "-> ";
        }
        else if (token.value.has_value()) {
            std::cout << token.value.value() << " ";
        } else {
            std::cout << token_type_str << " ";
        }
    }

    std::cout << std::endl;*/

    Parser parser(tokens);
    std::cout << "Parsing..." << std::endl;

    // Begin Parsing

    auto ast = parser.parseProgram();
    
    /*
        TODO: Implement this analyzing logic.
        Now this essentially just returns the tree
    */
    ast = analyzeTree(std::move(ast));

    CodeGenerator codegen;
    codegen.generate(*ast);

    

    //printPrettyTree(ast.get());
    return 0;
}
