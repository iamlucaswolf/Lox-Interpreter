//
// Created by Lucas Wolf on 2019-02-26.
//

#include "RuntimeError.h"

using namespace std;

RuntimeError::RuntimeError(const Token &token, string message) : token{token}, message{move(message) } {}

string RuntimeError::what() const noexcept {
    return report (token.line, "", message);
}