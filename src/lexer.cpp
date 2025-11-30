#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream> // For std::cerr

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    const size_t len = source.length();

    while (pos < len) {
        char c = source[pos];

        // 1. Skip Carriage Returns (Windows compatibility)
        if (c == '\r') {
            pos++;
            continue;
        }

        /** IDENTIFIER */
        if (std::isalpha(static_cast<unsigned char>(c))) {
            std::string id;
            // FIX: Added parenthesis around (isalnum || _) to prevent buffer overflow
            while (pos < len && (std::isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_')) {
                id += source[pos++];
            }
            tokens.push_back({TokenType::IDENTIFIER, id, line});
            continue;
        }

        /** NUMBER */
        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::string number;
            while (pos < len && std::isdigit(static_cast<unsigned char>(source[pos]))) {
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
            /** AT_KEYWORDS **/
            case '@': {
                ++pos;
                if (pos >= len) break;

                // Handle @identifier / @keyword
                if (std::isalpha(static_cast<unsigned char>(source[pos]))) {
                    std::string name;
                    // FIX: Added parenthesis here as well
                    while (pos < len && (std::isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_')) {
                        name += source[pos++];
                    }

                    // keyword or identifier?
                    if (BUILTIN_TOKENS.find(name) != BUILTIN_TOKENS.end()) {
                        tokens.push_back({BUILTIN_TOKENS.at(name), name, line});
                    } else {
                        tokens.push_back({TokenType::AT_IDENTIFIER, name, line});
                    }

                    continue;
                }
                break;
            }

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
            case '{':
            case '}':
                pos++; // Skipping for now
                continue;
        }

        /** UNKNOWN CHARACTER */
        std::cerr << "Unknown character at line " << line << ": '" << c << "' (" << (int)c << ")" << std::endl;
        pos++;
    }

    tokens.push_back({TokenType::END_OF_FILE, std::nullopt, line});
    return tokens;
}
