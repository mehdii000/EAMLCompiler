#include <iostream>
#include <vector>

#include "codeutils.hpp"
#include "lexer.hpp"

int main(int argc, char const *argv[])
{
    std::string source = readFile(argv[1]);
    
    std::cout << "====== LEXING =======" << std::endl;
    std::cout << source;
    std::cout << "=====================" << std::endl;

    std::vector<Token> tokens;
    Lexer lexer(source);
    tokens = lexer.tokenize();

    return 0;
}
