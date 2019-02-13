//
// Created by Lucas Wolf on 2019-01-20.
//

#ifndef LOX_INTERPRETER_SCANNER_H
#define LOX_INTERPRETER_SCANNER_H

#include "token.h"

#include <string>
#include <vector>


std::vector<Token> scan(const std::string &source);

class SyntaxError : public std::invalid_argument {
public:

    enum class Type {
        UNEXPECTED_CHARACTER,
        UNTERMINATED_STRING
    };

    SyntaxError(int line, const Type &type);


private:
    static std::string init_super(int line, const Type &Type);
};

using ErrorType = SyntaxError::Type;

#endif //LOX_INTERPRETER_SCANNER_H
