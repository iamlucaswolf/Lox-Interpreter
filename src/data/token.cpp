//
// Created by Lucas Wolf on 2019-01-24.
//

#include "token.h"

using namespace std;

Token::Token(Token::Type type, std::string &&lexeme, int line)
    : type{type}, lexeme{std::move(lexeme)}, line{line} {}

Token_ptr Token::New(Token::Type type, std::string &&lexeme, int line) {
    return make_shared<Token>(type, move(lexeme), line);
}


ostream& operator<<(ostream &out, const TokenType &type) {

    // The monstrous (and somewhat stupid) switch-statement below could have been avoided by using common enum
    // replacements from libraries such as e.g. "better enums" (http://aantron.github.io/better-enums/index.html).
    // I decided against that in order to keep the project free of additional dependencies.

    switch (type) {
        case TokenType::LEFT_PAREN:
            out << "LEFT_PAREN";
            break;
        case TokenType::RIGHT_PAREN:
            out << "RIGHT_PAREN";
            break;
        case TokenType::LEFT_BRACE:
            out << "LEFT_BRACE";
            break;
        case TokenType::RIGHT_BRACE:
            out << "RIGHT_BRACE";
            break;
        case TokenType::COMMA:
            out << "COMMA";
            break;
        case TokenType::DOT:
            out << "DOT";
            break;
        case TokenType::MINUS:
            out << "MINUS";
            break;
        case TokenType::PLUS:
            out << "PLUS";
            break;
        case TokenType::SEMICOLON:
            out << "SEMICOLON";
            break;
        case TokenType::SLASH:
            out << "SLASH";
            break;
        case TokenType::STAR:
            out << "STAR";
            break;
        case TokenType::BANG:
            out << "BANG";
            break;
        case TokenType::BANG_EQUAL:
            out << "BANG_EQUAL";
            break;
        case TokenType::EQUAL:
            out << "EQUAL";
            break;
        case TokenType::EQUAL_EQUAL:
            out << "EQUAL_EQUAL";
            break;
        case TokenType::GREATER:
            out << "GREATER";
            break;
        case TokenType::GREATER_EQUAL:
            out << "GREATER_EQUAL";
            break;
        case TokenType::LESS:
            out << "LESS";
            break;
        case TokenType::LESS_EQUAL:
            out << "LESS_EQUAL";
            break;
        case TokenType::IDENTIFIER:
            out << "IDENTIFIER";
            break;
        case TokenType::STRING:
            out << "STRING";
            break;
        case TokenType::NUMBER:
            out << "NUMBER";
            break;
        case TokenType::AND:
            out << "AND";
            break;
        case TokenType::CLASS:
            out << "CLASS";
            break;
        case TokenType::ELSE:
            out << "ELSE";
            break;
        case TokenType::FALSE:
            out << "FALSE";
            break;
        case TokenType::FUN:
            out << "FUN";
            break;
        case TokenType::FOR:
            out << "FOR";
            break;
        case TokenType::IF:
            out << "IF";
            break;
        case TokenType::NIL:
            out << "NIL";
            break;
        case TokenType::OR:
            out << "OR";
            break;
        case TokenType::PRINT:
            out << "PRINT";
            break;
        case TokenType::RETURN:
            out << "RETURN";
            break;
        case TokenType::SUPER:
            out << "SUPER";
            break;
        case TokenType::THIS:
            out << "THIS";
            break;
        case TokenType::TRUE:
            out << "TRUE";
            break;
        case TokenType::VAR:
            out << "VAR";
            break;
        case TokenType::WHILE:
            out << "WHILE";
            break;
        case TokenType::END_OF_FILE:
            out << "EOF";
            break;
    }

    return out;
}

ostream &operator<<(ostream &out, const Token &token) {
    return out << token.type << " " << token.lexeme;
}