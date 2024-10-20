#include <iostream>
#include <fstream>
#include "Frontend/Scanner.hpp"
#include "Frontend//SYNTAX_ANALYZER.hpp"

std::vector<Token> scanFile(const std::string& filename) {
    std::vector<Token> parsedTokens;  // To store all tokens
    Scanner scanner;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filename << std::endl;
        return parsedTokens;
    }

    std::string line;
    int lineNumber = 1;
    while (std::getline(file, line)) {
        std::vector<Token> tokens = scanner.scan(line, lineNumber);
        for (const Token& token : tokens) {
            parsedTokens.push_back(token);  // Store each token individually
        }
        lineNumber++;
    }

    file.close();

    // Example: Displaying all parsed tokens after file scanning
    std::cout << "\nAll parsed tokens:\n";
    for (const Token& token : parsedTokens) {
        std::cout << "Token: " << token.value << ", Type: " << token.getTypeAsString() << ", Line: " << token.line << std::endl;
    }
    return parsedTokens;

}

int main() {
    std::string filename = "test.hol";
    std::vector<Token> parsedTokens = scanFile(filename);

    SyntaxAnalyzer parser(parsedTokens);
    parser.parseProgram();
    return 0;
}
