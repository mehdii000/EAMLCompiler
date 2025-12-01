#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>

#include "codeutils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "anaylzer.hpp"
#include "codegen.hpp"

using namespace std::chrono_literals;
namespace fs = std::filesystem;

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
    //std::cout << "=========================" << std::endl;
    std::cout << "Tokenizing..." << std::endl;

    std::vector<Token> tokens;
    Lexer lexer(source);
    tokens = lexer.tokenize();

    /*
    for (const Token& token : tokens) {
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
    */

    std::cout << std::endl;

    Parser parser(tokens);
    //std::cout << "=========================" << std::endl;
    std::cout << "Parsing..." << std::endl;

    // Begin Parsing

    auto ast = parser.parseProgram();
    
    /*
        TODO: Implement this analyzing logic.
        Now this essentially just returns the tree
    */
    ast = analyzeTree(std::move(ast));
    //std::cout << "=========HTML============" << std::endl;

    CodeGenerator codegen;
    codegen.generate(*ast);
    //std::cout << "=========================" << std::endl;

    if (argc > 2 && std::string(argv[2]) == "-dev") {

        std::string path = argv[1];
        auto lastWrite = fs::last_write_time(path);

        while (true) {

            auto currentWrite = fs::last_write_time(path);

            if (currentWrite != lastWrite) {
                lastWrite = currentWrite;

                source = readFile(path.c_str());
                Lexer lexer_dev(source);
                std::vector<Token> tokens_dev = lexer_dev.tokenize();
                Parser parser_dev(tokens_dev);
                auto ast_dev = parser_dev.parseProgram();
                ast_dev = analyzeTree(std::move(ast_dev));
                codegen.generate(*ast_dev);
                printPrettyTree(ast_dev.get());
                std::cout << std::endl;
                std::cout << "Press any Ctrl+C to exit" << std::endl;
            }

            std::this_thread::sleep_for(100ms); // IMPORTANT
        }
    }

    return 0;
}
