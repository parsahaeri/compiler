#ifndef DFA_HPP
#define DFA_HPP

#include <regex>

enum class State {
    START,
    IN_KEYWORD,
    IN_NUMBER,
    IN_OPERATOR,
    IN_DELIMITER,
    DONE
};

class DFA {
public:
    DFA() : currentState(State::START) {
        initializeRegexPatterns();
    }

    [[nodiscard]] State getCurrentState() const {
        return currentState;
    }

    void transition(char ch) {
        switch (currentState) {
            case State::START:
                if (isRegexMatch(std::string(1, ch), operatorRegex)) {
                    currentState = State::IN_OPERATOR;
                } else if (isRegexMatch(std::string(1, ch), numberRegex)) {
                    currentState = State::IN_NUMBER;
                } else if (isRegexMatch(std::string(1, ch), identifierRegex)) {
                    currentState = State::IN_KEYWORD;
                } else if (isRegexMatch(std::string(1, ch), delimiterRegex)) {
                    currentState = State::IN_DELIMITER;
                } else {
                    currentState = State::START;
                }
                break;

            case State::IN_KEYWORD:
                if (isRegexMatch(std::string(1, ch), identifierRegex)) {
                    currentState = State::IN_KEYWORD;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_NUMBER:
                if (isRegexMatch(std::string(1, ch), numberRegex)) {
                    currentState = State::IN_NUMBER;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_OPERATOR:
                tempOperator += ch;
                if (isRegexMatch(tempOperator, operatorRegex)) {
                    currentState = State::IN_OPERATOR;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_DELIMITER:
                currentState = State::DONE;
                break;

            default:
                currentState = State::START;
                break;
        }
    }

    void reset() {
        currentState = State::START;
        tempOperator.clear();
    }

    const std::string& getTempOperator() const {
        return tempOperator;
    }

    bool isDelimiterChar(char ch) {
        return isRegexMatch(std::string(1, ch), delimiterRegex);
    }

private:
    State currentState;
    std::string tempOperator;  // Store the operator string being built

    // Regular expressions for different token types
    std::regex identifierRegex;
    std::regex numberRegex;
    std::regex operatorRegex;
    std::regex delimiterRegex;

    // Initialize regex patterns
    void initializeRegexPatterns() {
        identifierRegex = std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$");  // Identifiers
        numberRegex = std::regex("^[0-9]+(\\.[0-9]+)?$");  // Numbers
        operatorRegex = std::regex("^[+\\-*/=<>!&|^%]+$");  // Operators (math/logical)
        delimiterRegex = std::regex(R"([(){}[\];,])");  // Delimiters (can extend as needed)
    }

    // Check if a string matches a regex pattern
    static bool isRegexMatch(const std::string& value, const std::regex& pattern) {
        return std::regex_match(value, pattern);
    }
};

#endif