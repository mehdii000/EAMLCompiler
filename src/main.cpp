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

template <typename F>
void BENCHMARK(F&& func, const std::string& action) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << action << " took " << duration.count() << "ms\n";
}

void run(const char* path) {
    std::string source = readFile(path);

    std::vector<Token> tokens;
    Lexer lexer(source);

    BENCHMARK([&]() { tokens = lexer.tokenize(); }, "Lexing");

    std::unique_ptr<RootNode> ast = nullptr;
    Parser parser(tokens);

    BENCHMARK([&]() { ast = parser.parseProgram(); }, "Parsing");

    BENCHMARK([&]() { ast = analyzeTree(std::move(ast)); }, "Analyzing AST");

    CodeGenerator codegen;
    BENCHMARK([&]() { codegen.generate(*ast); }, "Generating Code");

    printPrettyTree(ast.get());

    std::cout << "Exported to output.html\n";
}

int main(int argc, char const *argv[]) {
    if (argc < 2) return 1;

    const char* path = argv[1];
    run(path);

    if (argc > 2 && std::string(argv[2]) == "-dev") {
        auto lastWrite = fs::last_write_time(path);
        while (true) {
            auto currentWrite = fs::last_write_time(path);
            if (currentWrite != lastWrite) {
                lastWrite = currentWrite;
                run(path);
            }
            std::this_thread::sleep_for(250ms);
        }
    }
}
