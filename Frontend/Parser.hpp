#ifndef PARSER_HPP
#define PARSER_HPP


#include <iostream>
#include <stack>
#include <vector>
#include "Token.hpp"

enum class NonTerminal {S, VarDec, OptAssign, Expr};
enum class Terminal {TYPE, IDENTIFIER, NUMBER, ASSIGN, SEMICOLON, END};

inline Terminal getTerminal(const Token& token) {
    if(token.type == TokenType::KEYWORD) return Terminal::TYPE;
    if(token.type == TokenType::IDENTIFIER) return Terminal::IDENTIFIER;
    if(token.type == TokenType::NUMBER) return Terminal::NUMBER;
    if(token.value == "=") return Terminal::ASSIGN;
    if(token.value == ";") return Terminal::SEMICOLON;

    return Terminal::END;
}

class Parser {
public:
    explicit  Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0){}

    bool Parse() {
        parseStack.push(NonTerminal::S);

        while(!parseStack.empty()) {
            if(parseStack.top() == NonTerminal::S) {
                parseStack.pop();
                parseVarDec();
            }else {
                if(!parseNext()) return false;
            }
        }
        return true;
    }

private:
    const std::vector<Token>& tokens;
    int currentTokenIndex;
    std::stack<NonTerminal> parseStack;

    [[nodiscard]] Token currentToken() const {
        return tokens[currentTokenIndex];
    }

    void advance() {
        if(currentTokenIndex<tokens.size() - 1) currentTokenIndex++;
    }

    bool match(Terminal expected) {
        if(getTerminal((currentToken())) == expected) {
            advance();
            return true;
        }else {
            std::cerr << "Syntax Error: Expected " << getTerminalName(expected) << "but got " << currentToken().value << std::endl;
            return false;
        }
    }

    static std::string getTerminalName(Terminal terminal) {
        switch (terminal) {
            case Terminal::TYPE: return "Type";
            case Terminal::IDENTIFIER: return "Identifier";
            case Terminal::ASSIGN: return "=";
            case Terminal::NUMBER: return "Number";
            case Terminal::SEMICOLON: return ";";
            case Terminal::END: return "End";
            default: return "Unknown";
        }
    }

    void parseVarDec() {
        if(match(Terminal::TYPE)) {
            if(match(Terminal::IDENTIFIER)) {
                parseOptAssign();
                match(Terminal::SEMICOLON);
            }
        }
    }

    void parseOptAssign() {
        if(getTerminal(currentToken()) == Terminal::ASSIGN) {
            match(Terminal::ASSIGN);
            parseExpr();
        }
    }

    void parseExpr() {
        Terminal termType = getTerminal(currentToken());
        if(termType == Terminal::IDENTIFIER || termType == Terminal::NUMBER) {
            advance();
        }else {
            std::cerr << "Syntax Error: Expected identifier or numer" << std::endl;
        }
    }

    bool parseNext() {
        NonTerminal top = parseStack.top();
        parseStack.pop();

        switch (top) {
            case NonTerminal::VarDec:
                parseVarDec();
                break;
            case NonTerminal::OptAssign:
                parseOptAssign();
                break;
            case NonTerminal::Expr:
                parseExpr();
                break;
            default:
                std::cerr << "Syntax Error: Unexpected Item" << std::endl;
            return false;
        }

        return true;
    }

};

#endif //PARSER_HPP
