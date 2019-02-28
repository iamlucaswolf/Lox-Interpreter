//
// Created by Lucas Wolf on 2019-01-24.
//

#ifndef LOX_INTERPRETER_TOKEN_H
#define LOX_INTERPRETER_TOKEN_H

#include <string>
#include <iostream>
#include <memory>

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

    // NOTE in Nystorm's implementation, the Token class contains an `LoxObject literal` member that holds the parsed
    // literal for appropriate types (i.e. identifier, string, number). We don't do this to avoid engineering headaches
    // and rather let the interpreter figure out the literal value based on the lexeme

    const Type type;
    const std::string lexeme;
    const int line;

    Token(Type type, std::string lexeme, int line) : type{type}, lexeme{std::move(lexeme)}, line{line} {};
};

using TokenType = Token::Type;
using Token_ptr = std::unique_ptr<Token>;

std::ostream& operator<<(std::ostream &out, const Token &token);
std::ostream& operator<<(std::ostream &out, const TokenType &type);

#endif //LOX_INTERPRETER_TOKEN_H
