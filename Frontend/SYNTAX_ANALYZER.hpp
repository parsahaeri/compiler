#ifndef SYNTAX_ANALYZER_HPP
#define SYNTAX_ANALYZER_HPP
#include <iostream>
#include <vector>
#include <string>
#include "Token.hpp"  // Assume Token.hpp is defined as in your original code

class Parser {
    std::vector<Token> tokens;
    int current;

public:
    explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

    Token peek() {
        return tokens[current];
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Token previous() {
        return tokens[current - 1];
    }

    bool isAtEnd() {
        return peek().type == TokenType::DELIMITER && peek().value == "}";
    }

    bool match(TokenType type, const std::string& value = "") {
        if (peek().type != type) return false;
        if (!value.empty() && peek().value != value) return false;
        advance();
        return true;
    }

    void parse() {
        try {
            while (!isAtEnd()) {
                statement();
            }
        } catch (const std::string &error) {
            std::cerr << "Syntax Error: " << error << std::endl;
        }
    }

    void statement() {
        if (match(TokenType::KEYWORD, "int")) {
            declaration();
        } else if (match(TokenType::KEYWORD, "if")) {
            ifStatement();
        } else if (match(TokenType::KEYWORD, "while")) {
            whileStatement();
        } else {
            assignment();
        }
    }

    void declaration() {
        // Expecting something like: int x = 5;
        if (!match(TokenType::IDENTIFIER)) {
            throw "Expected identifier after type.";
        }
        if (!match(TokenType::OPERATOR, "=")) {
            throw "Expected '=' in declaration.";
        }
        expression();
        if (!match(TokenType::DELIMITER, ";")) {
            throw "Expected ';' after declaration.";
        }
    }

    void assignment() {
        if (!match(TokenType::IDENTIFIER)) {
            throw "Expected identifier.";
        }
        if (!match(TokenType::OPERATOR, "=")) {
            throw "Expected '=' in assignment.";
        }
        expression();
        if (!match(TokenType::DELIMITER, ";")) {
            throw "Expected ';' after assignment.";
        }
    }

    void ifStatement() {
        if (!match(TokenType::DELIMITER, "(")) {
            throw "Expected '(' after 'if'.";
        }
        expression();
        if (!match(TokenType::DELIMITER, ")")) {
            throw "Expected ')' after condition.";
        }
        if (!match(TokenType::DELIMITER, "{")) {
            throw "Expected '{' to open 'if' block.";
        }
        statementList();
        if (!match(TokenType::DELIMITER, "}")) {
            throw "Expected '}' to close 'if' block.";
        }

        // Check for optional else
        if (match(TokenType::KEYWORD, "else")) {
            if (!match(TokenType::DELIMITER, "{")) {
                throw "Expected '{' to open 'else' block.";
            }
            statementList();
            if (!match(TokenType::DELIMITER, "}")) {
                throw "Expected '}' to close 'else' block.";
            }
        }
    }

    void whileStatement() {
        if (!match(TokenType::DELIMITER, "(")) {
            throw "Expected '(' after 'while'.";
        }
        expression();
        if (!match(TokenType::DELIMITER, ")")) {
            throw "Expected ')' after condition.";
        }
        if (!match(TokenType::DELIMITER, "{")) {
            throw "Expected '{' to open 'while' block.";
        }
        statementList();
        if (!match(TokenType::DELIMITER, "}")) {
            throw "Expected '}' to close 'while' block.";
        }
    }

    void expression() {
        // Handle a basic expression of the form Term Operator Term
        term();
        if (match(TokenType::OPERATOR)) {
            term();
        }
    }

    void term() {
        if (match(TokenType::IDENTIFIER) || match(TokenType::NUMBER)) {
            return;
        } else {
            throw "Expected identifier or number.";
        }
    }

    void statementList() {
        while (!match(TokenType::DELIMITER, "}")) {
            statement();
        }
    }
};


#endif // SYNTAX_ANALYZER_HPP
