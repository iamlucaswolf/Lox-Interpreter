//
// Created by Lucas Wolf on 2019-01-24.
//

#ifndef LOX_INTERPRETER_TOKEN_H
#define LOX_INTERPRETER_TOKEN_H

#include <string>
#include <iostream>
#include <variant>

struct Token {

    enum class Type {
        // NOTE The C++ Core Guidelines explicitly advise to use lowercase identifiers for enum members (rule Enum.5). We
        // deliberately break this rule, since most keywords in Lox are reserved words in C++.

        // Single-character tokens
                LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        // One or two character tokens
                BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

        // Literals
                IDENTIFIER, STRING, NUMBER,

        // Keywords
                AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, END_OF_FILE
    };


    const Type type;
    const std::string lexeme;
    const int line;

    // NOTE in Nystorm's implementation, the Token class contains an `Object literal` member that holds the parsed
    // literal for appropriate types (i.e. identifier, string, number). We don't do this to avoid engineering headaches
    // and rather let the interpreter figure out the literal value based on the lexeme
};

using TokenType = Token::Type;

std::ostream& operator<<(std::ostream &out, const Token &token);
std::ostream& operator<<(std::ostream &out, const Token::Type &type);

#endif //LOX_INTERPRETER_TOKEN_H
