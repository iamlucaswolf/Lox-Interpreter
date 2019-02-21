//
// Created by Lucas Wolf on 2019-02-14.
//

#ifndef LOX_INTERPRETER_PARSER_H
#define LOX_INTERPRETER_PARSER_H

#include <vector>

#include "error.h"
#include "data/token.h"
#include "data/ast.h"

std::unique_ptr<Expression> parse(std::vector<std::shared_ptr<Token>> &tokens);

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
