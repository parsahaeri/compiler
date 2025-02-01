#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

struct Token {
    string type;
    string value;
    int line;
    int column;
};

class LexicalAnalyzer {
private:
    vector<Token> tokens;
    string delimiters = " \\t\\n\\r,;(){}=+-<>";

    bool isIdentifier(const string &word) {
        return regex_match(word, regex("^[a-zA-Z]{1,5}$"));
    }

    bool isInteger(const string &word) {
        return regex_match(word, regex("^[0-9]+$"));
    }

    string getTokenType(const string &word) {
        if (isIdentifier(word)) return "Identifier";
        if (isInteger(word)) return "Integer";
        if (word == "If" || word == "Iteration" || word == "Put" || word == "Read" || word == "Print" || word == "Var") return "Keyword";
        if (word == "+" || word == "-" || word == "=" || word == ">" || word == "<" || word == "==") return "Operator";
        return "Unknown";
    }

public:
    void analyze(const string &code) {
        tokens.clear();
        string token;
        int line = 1, column = 0;
        for (size_t i = 0; i < code.size(); ++i) {
            char c = code[i];
            column++;
            if (delimiters.find(c) != string::npos) {
                if (!token.empty()) {
                    string type = getTokenType(token);
                    tokens.push_back({type, token, line, column - static_cast<int>(token.size())});
                    token.clear();
                }
                if (c != ' ' && c != '\\t' && c != '\\n' && c != '\\r') {
                    string symbol(1, c);
                    tokens.push_back({"Symbol", symbol, line, column});
                }
                if (c == '\\n') {
                    line++;
                    column = 0;
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            string type = getTokenType(token);
            tokens.push_back({type, token, line, column - static_cast<int>(token.size())});
        }
    }

    const vector<Token> &getTokens() const {
        return tokens;
    }

    void printTokens() const {
        for (const auto &token : tokens) {
            cout << "[Type: " << token.type << ", Value: " << token.value << ", Line: " << token.line << ", Column: " << token.column << "]\n";
        }
    }
};

