#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    const size_t len = source.length();

    while (pos < len) {
        char c = source[pos];

        /** IDENTIFIER */
        if (std::isalpha(c)) {
            std::string id;
            while (pos < len && std::isalnum(source[pos])) {
                id += source[pos++];
            }
            tokens.push_back({TokenType::IDENTIFIER, id, line});
            continue;
        }

        /** NUMBER */
        if (std::isdigit(c)) {
            std::string number;
            while (pos < len && std::isdigit(source[pos])) {
                number += source[pos++];
            }
            tokens.push_back({TokenType::NUMBER, number, line});
            continue;
        }

        /** INDENTATION (4 spaces) */
        if (c == ' ') {
            if (pos + 3 < len &&
                source[pos+1] == ' ' &&
                source[pos+2] == ' ' &&
                source[pos+3] == ' ') 
            {
                tokens.push_back({TokenType::INDENT, std::nullopt, line});
                pos += 4;
                continue;
            }

            // Single spaces are ignored
            pos++;
            continue;
        }

        /** NEWLINE */
        if (c == '\n') {
            tokens.push_back({TokenType::NEWLINE, std::nullopt, line});
            pos++;
            line++;
            continue;
        }

        /** STRINGS */
        if (c == '"') {
            pos++; // skip opening "
            std::string value;

            while (pos < len) {
                char s = source[pos];

                // escape support \" and \\ 
                if (s == '\\' && pos + 1 < len) {
                    char next = source[pos + 1];
                    if (next == '"' || next == '\\') {
                        value += next;
                        pos += 2;
                        continue;
                    }
                }

                if (s == '"') break;

                value += s;
                pos++;
            }

            if (pos >= len)
                throw std::runtime_error("Unterminated string literal");

            pos++; // skip closing "
            tokens.push_back({TokenType::STRING, value, line});
            continue;
        }

        /** SINGLE-CHAR TOKENS */
        switch (c) {
            case '@':
                tokens.push_back({TokenType::AT_SYMBOL, "@", line});
                pos++;
                continue;

            case ':':
                tokens.push_back({TokenType::COLON, ":", line});
                pos++;
                continue;

            case ',':
                tokens.push_back({TokenType::COMMA, ",", line});
                pos++;
                continue;

            case '[':
                tokens.push_back({TokenType::LBRACKET, "[", line});
                pos++;
                continue;

            case ']':
                tokens.push_back({TokenType::RBRACKET, "]", line});
                pos++;
                continue;
        }

        /** UNKNOWN CHARACTER */
        throw std::runtime_error(
            std::string("Unexpected character '") + c +
            "' at line " + std::to_string(line)
        );
    }

    tokens.push_back({TokenType::END_OF_FILE, std::nullopt, line});
    return tokens;
}
