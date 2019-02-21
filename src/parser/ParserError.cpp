//
// Created by Lucas Wolf on 2019-02-19.
//

#include "parser.h"

#include <sstream>

using namespace std;

// ParserError

ParserError::ParserError(vector<ParseError> &&errors) : errors{ move(errors) } {}

string ParserError::what() const noexcept {
    stringstream buffer;

    for (auto e = errors.begin(); e != errors.end(); ++e) {
        buffer << e->what();

        if (e != errors.end() - 1) {
            buffer << "\n";
        }
    }

    return buffer.str();
}


// ParseError

ParseError::ParseError(const Token &token, string message) : token{token}, message{ std::move(message) } {};

string ParseError::what() const noexcept {
    return report(
            token.line,
            token.type == TokenType::END_OF_FILE ? " at end" : "at '" + token.lexeme + "'",
            message
    );
}