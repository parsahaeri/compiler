#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <vector>
#include <algorithm>
#include "Token.hpp"
#include "Keywords.hpp"
#include "DFA.hpp"


class Scanner {
public:
    Scanner() : dfa() {}
    std::vector<Token> scan(const std::string& line, int& lineNumber) {
        std::vector<Token> tokens;
        std::string tokenValue;
        dfa.reset();

        for (char ch : line) {
            if (std::isspace(ch)) {
                if (!tokenValue.empty()) {
                    Token token = processToken(tokenValue, lineNumber);
                    tokens.push_back(token);
                    tokenValue.clear();
                }
                dfa.reset();
                continue;
            }

            // Check if the character is a delimiter first
            if (dfa.isDelimiterChar(ch)) {
                if (!tokenValue.empty()) {
                    Token token = processToken(tokenValue, lineNumber);
                    tokens.push_back(token);
                    tokenValue.clear();
                }

                Token token = processToken(std::string(1, ch), lineNumber, TokenType::DELIMITER);
                tokens.push_back(token);
                dfa.reset();
                continue;
            }

            // Transition the DFA with the current character
            dfa.transition(ch);
            tokenValue += ch;

            // Process token if DFA reaches a final state
            if (dfa.getCurrentState() == State::DONE) {
                // Determine the token type based on the last recognized state
                TokenType type = identifyTokenType(tokenValue);
                Token token = {type, tokenValue, lineNumber};
                tokens.push_back(token);
                tokenValue.clear();
                dfa.reset();
            }
        }

        // Handle any remaining token at the end of the line
        if (!tokenValue.empty()) {
            Token token = processToken(tokenValue, lineNumber);
            tokens.push_back(token);
        }

        return tokens;
    }


private:
    DFA dfa;

    Token processToken(const std::string& value, int line, TokenType typeOverride = TokenType::KEYWORD) {
        TokenType type;

        if (typeOverride == TokenType::KEYWORD) {
            type = identifyTokenType(value);
        } else {
            type = typeOverride;
        }

        return {type, value, line};
    }

    TokenType identifyTokenType(const std::string& value) {
        // Use the state of the DFA to identify the token type
        switch (dfa.getCurrentState()) {
            case State::IN_KEYWORD:
                if (isKeyword(value)) {
                    return TokenType::KEYWORD;
                }
            break;

            case State::IN_NUMBER:
                return TokenType::NUMBER;

            case State::IN_OPERATOR:
                return TokenType::OPERATOR;

            case State::IN_DELIMITER:
                return TokenType::DELIMITER;

            default:
                break;
        }
        return TokenType::IDENTIFIER;
    }

    static bool isKeyword(const std::string& value) {
        return std::find(Keywords::getKeywords().begin(), Keywords::getKeywords().end(), value) != Keywords::getKeywords().end();
    }

};

#endif
