#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
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
    string delimiters = " \t\n\r,;(){}=+-<>";

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
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                    string symbol(1, c);
                    tokens.push_back({"Symbol", symbol, line, column});
                }
                if (c == '\n') {
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
};

struct ASTNode {
    string value;
    vector<shared_ptr<ASTNode>> children;
    ASTNode(const string &val) : value(val) {}
};

class Parser {
private:
    vector<Token> tokens;
    int currentIndex;
    Token currentToken;

    void advance() {
        if (currentIndex < tokens.size()) {
            currentToken = tokens[currentIndex++];
        }
    }

    bool match(const string &type) {
        return currentToken.type == type;
    }

    void expected(const string &expectedToken) {
        cerr << "Syntax Error: Expected " << expectedToken << ", found '" << currentToken.value << "' at line " << currentToken.line << ", column " << currentToken.column << endl;
        exit(1);
    }

    shared_ptr<ASTNode> parseExpr() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Expr");
        node->children.push_back(parseR());
        node->children.push_back(parseExprPrime());
        return node;
    }

    shared_ptr<ASTNode> parseExprPrime() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Expr'");
        if (match("Operator") && (currentToken.value == "+" || currentToken.value == "-")) {
            node->children.push_back(make_shared<ASTNode>(currentToken.value));
            advance();
            node->children.push_back(parseR());
            node->children.push_back(parseExprPrime());
        }
        return node;
    }

    shared_ptr<ASTNode> parseStates() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("States");
        node->children.push_back(parseState());
        node->children.push_back(parseStatesPrime());
        return node;
    }

    shared_ptr<ASTNode> parseStatesPrime() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("States'");
        if (match("Keyword") || match("Identifier")) {
            node->children.push_back(parseStates());
        }
        return node;
    }

    shared_ptr<ASTNode> parseR() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("R");
        if (match("Identifier") || match("Integer")) {
            node->children.push_back(make_shared<ASTNode>(currentToken.value));
            advance();
        } else {
            expected("Identifier or Integer");
        }
        return node;
    }

public:
    Parser(const vector<Token> &tokens) : tokens(tokens), currentIndex(0) {
        advance();
    }

    shared_ptr<ASTNode> parse() {
        return parseStates();
    }
};

int main() {
    string code;
    cout << "Enter the file path: ";
    string filePath;
    cin >> filePath;

    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        code += line + '\n';
    }
    inputFile.close();

    LexicalAnalyzer lexicalAnalyzer;
    lexicalAnalyzer.analyze(code);
    const vector<Token> &tokens = lexicalAnalyzer.getTokens();

    Parser parser(tokens);
    shared_ptr<ASTNode> ast = parser.parse();

    cout << "Parsing completed successfully!" << endl;

    return 0;
}
