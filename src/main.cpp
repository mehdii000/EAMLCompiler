#include <iostream>
#include "codeutils.hpp"


int main(int argc, char const *argv[])
{
    std::string source = readFile(argv[1]);
    std::cout << source << std::endl;
    
    return 0;
}
