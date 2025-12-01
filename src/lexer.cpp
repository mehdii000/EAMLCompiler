#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    const size_t len = source.length();

    while (pos < len) {
        char c = source[pos];

        // Ignore CR for Windows compatibility
        if (c == '\r') {
            pos++;
            continue;
        }

        /** IDENTIFIER */
        if (std::isalpha(static_cast<unsigned char>(c))) {
            std::string id;

            while (pos < len &&
                   (std::isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_'))
            {
                id += source[pos++];
            }

            auto it_builtin = BUILTIN_TOKENS.find(id);
            if (it_builtin != BUILTIN_TOKENS.end()) {
                tokens.push_back(Token{it_builtin->second, id, line});
            } else {
                tokens.push_back(Token{TokenType::IDENTIFIER, id, line});
            }

            continue;
        }

        /** NUMBER */
        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::string number;

            while (pos < len &&
                   std::isdigit(static_cast<unsigned char>(source[pos])))
            {
                number += source[pos++];
            }

            tokens.push_back(Token{TokenType::NUMBER, number, line});
            continue;
        }

        /** INDENTATION (4 spaces = INDENT token) */
        if (c == ' ') {
            if (pos + 3 < len &&
                source[pos+1] == ' ' &&
                source[pos+2] == ' ' &&
                source[pos+3] == ' ')
            {
                tokens.push_back(Token{TokenType::INDENT, std::nullopt, line});
                pos += 4;
                continue;
            }

            pos++;
            continue;
        }

        /** NEWLINES (collapse multiple into one token) */
        if (c == '\n') {
            pos++;
            line++;

            while (pos < len && source[pos] == '\n') {
                pos++;
                line++;
            }

            tokens.push_back(Token{TokenType::NEWLINE, std::nullopt, line});
            continue;
        }

        /** STRINGS */
        if (c == '"') {
            pos++; // skip "
            std::string value;

            while (pos < len) {
                char s = source[pos];

                // escape sequences \" and
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
            tokens.push_back(Token{TokenType::STRING, value, line});
            continue;
        }

        /** SINGLE-CHAR + AT-KEYWORDS */
        switch (c) {

            case '@': {
                pos++;
                if (pos >= len) {
                    tokens.push_back(Token{TokenType::AT_IDENTIFIER, "", line});
                    break;
                }

                if (std::isalpha(static_cast<unsigned char>(source[pos]))) {
                    std::string name;

                    while (pos < len &&
                           (std::isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_'))
                    {
                        name += source[pos++];
                    }

                    auto it_at = BUILTIN_AT_TOKENS.find(name);
                    if (it_at != BUILTIN_AT_TOKENS.end()) {
                        tokens.push_back(Token{it_at->second, name, line});
                    } else {
                        tokens.push_back(Token{TokenType::AT_IDENTIFIER, name, line});
                    }
                }

                break;
            }

            case ':':
                tokens.push_back(Token{TokenType::COLON, ":", line});
                pos++;
                break;

            case ',':
                tokens.push_back(Token{TokenType::COMMA, ",", line});
                pos++;
                break;

            case '[':
                tokens.push_back(Token{TokenType::LBRACKET, "[", line});
                pos++;
                break;

            case ']':
                tokens.push_back(Token{TokenType::RBRACKET, "]", line});
                pos++;
                break;

            case '#': {
                // comment until newline
                while (pos < len && source[pos] != '\n') pos++;
                break;
            }

            case '{':
            case '}':
                pos++;
                break;

            default:
                std::cerr << "Unknown character at line " << line
                          << ": '" << c << "' (" << int(c) << ")" << std::endl;
                pos++;
                break;
        }

        // Continue outer loop after switch
        continue;
    }

    tokens.push_back(Token{TokenType::END_OF_FILE, std::nullopt, line});
    return tokens;
}
