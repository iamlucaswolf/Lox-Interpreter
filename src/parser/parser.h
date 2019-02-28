//
// Created by Lucas Wolf on 2019-02-14.
//

#ifndef LOX_INTERPRETER_PARSER_H
#define LOX_INTERPRETER_PARSER_H

#include "error.h"
#include "data/token.h"
#include "data/expression.h"
#include "data/statement.h"

#include <vector>


// TODO move Statements to std::vector<>?
std::vector<Statement_ptr> parse(std::vector<Token_ptr> &&tokens);

struct ParseError : LoxError {
    const Token &token;
    const std::string message;

    ParseError(const Token &token, std::string message);
    std::string what() const noexcept override;

};

struct ParserError : LoxError {
    const std::vector<ParseError> errors;

    explicit ParserError(std::vector<ParseError> &&errors);
    std::string what() const noexcept override;
};

#endif //LOX_INTERPRETER_PARSER_H
