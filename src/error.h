//
// Created by Lucas Wolf on 2019-02-18.
//

#ifndef LOX_INTERPRETER_ERROR_H
#define LOX_INTERPRETER_ERROR_H

#include <exception>
#include <string>

struct LoxError {

    virtual std::string what() const noexcept = 0;

    static std::string report(int line, const std::string &where, const std::string &message) noexcept {
           return "[line " + std::to_string(line) + "] Error " + where + ": " + message;
    }
};

#endif //LOX_INTERPRETER_ERROR_H
