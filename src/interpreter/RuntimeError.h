//
// Created by Lucas Wolf on 2019-02-26.
//

#ifndef LOX_INTERPRETER_RUNTIMEERROR_H
#define LOX_INTERPRETER_RUNTIMEERROR_H

#include "data/token.h"
#include "error.h"

#include <string>

/*
 * Signals the occurence of an Error during execution.
 */
struct RuntimeError : LoxError {

    const Token &token;
    const std::string message;

    RuntimeError(const Token &token, std::string message);

    std::string what() const noexcept override;
};

#endif //LOX_INTERPRETER_RUNTIMEERROR_H
