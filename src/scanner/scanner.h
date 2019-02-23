//
// Created by Lucas Wolf on 2019-01-20.
//

#ifndef LOX_INTERPRETER_SCANNER_H
#define LOX_INTERPRETER_SCANNER_H

#include "data/token.h"

#include <string>
#include <vector>
#include <memory>
#include <error.h>


std::vector<std::shared_ptr<Token>> scan(const std::string &source);

struct ScanError : public LoxError {

    enum class Type {
        UNEXPECTED_CHARACTER,
        UNTERMINATED_STRING
    };

    int line;
    const Type type;

    ScanError(int line, const Type type) : line{line}, type{type} {};

    std::string what() const noexcept;
};

using ErrorType = ScanError::Type;

#endif //LOX_INTERPRETER_SCANNER_H
  //