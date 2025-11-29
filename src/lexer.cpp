#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    size_t sourceLength = source.length();

    while (pos < sourceLength) {
        char c = source[pos];

        if (std::isalpha(c)) {
            std::string identifier;
            while (pos < sourceLength && std::isalnum(source[pos])) {
                identifier += source[pos];
                pos++;
            }
            tokens.push_back({TokenType::IDENTIFIER, identifier, line});
            continue;
        } else if (std::isdigit(c)) {
            std::string number;
            while (pos < sourceLength && std::isdigit(source[pos])) {
                number += source[pos];
                pos++;
            }
            tokens.push_back({TokenType::NUMBER, number, line});
            continue;
        } else if (c == ' ') {
            // Make sure pos+3 is valid BEFORE accessing those indices
            if (pos + 3 < sourceLength &&
                source[pos+1] == ' ' &&
                source[pos+2] == ' ' &&
                source[pos+3] == ' ') {

                tokens.push_back({TokenType::INDENT, std::nullopt, line});
                pos += 3;
            }
        } else if (c == '\n') {
            tokens.push_back({TokenType::NEWLINE, std::nullopt, line});
            line++;
            pos++;
        } else if (c == '"') {
            std::string string;
            pos++;
            while (pos < sourceLength && source[pos] != '"') {
                string += source[pos];
                pos++;
            }
            if (pos == sourceLength) {
                throw std::runtime_error("Unterminated string");
            }
            tokens.push_back({TokenType::STRING, string, line});
            pos++;
        } else if (c == EOF) {
            tokens.push_back({TokenType::END_OF_FILE, std::nullopt, line});
            break;
        }


        // Single-character tokens
        switch (c) {
            case '@':
                tokens.push_back({TokenType::AT_SYMBOL, "@", line});
                pos++;
                break;
            case ':':
                tokens.push_back({TokenType::COLON, ":", line});
                pos++;
                break;
            case ',':
                tokens.push_back({TokenType::COMMA, ",", line});
                pos++;
                break;
            case '[':
                tokens.push_back({TokenType::LBRACKET, "[", line});
                pos++;
                break;
            case ']':
                tokens.push_back({TokenType::RBRACKET, "]", line});
                pos++;
                break;
            default:
                pos++;
                break;
        }
    }

    return tokens;
}
