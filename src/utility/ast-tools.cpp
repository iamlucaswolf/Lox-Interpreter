//
// Created by Lucas Wolf on 2019-02-13.
//

#include "data/expression.h"
#include "ast-tools.h"

using namespace std;


string Printer::print(Expression &expression) {
    buffer.str("");

    expression.accept(*this);

    return buffer.str();
}

void Printer::visit(const Literal &expression) {

    const string &lexeme = expression.token->lexeme;

    // TODO maybe this should be made a method of token, like variant<monostate, double, string> value() and then
    // converting the value back to string?
    switch (expression.token->type) {

        case Token::Type::NUMBER:
        case Token::Type::IDENTIFIER:
            buffer << lexeme;
            break;

        case Token::Type::STRING:
            buffer << lexeme.substr(1, lexeme.length() - 2);
            break;

        default:
            buffer << "nil";
    }
}

void Printer::visit(const Binary &expression)  {
    parenthesized(expression.token->lexeme, {expression.left.get(), expression.right.get()});
}

void Printer::visit(const Grouping &expression) {
    parenthesized("group", {expression.content.get()});
}

void Printer::visit(const Unary &expression) {
    parenthesized(expression.token->lexeme, {expression.operand.get()});
}

void Printer::parenthesized(string name, initializer_list<const Expression*> expressions) {
    buffer << "(" << name;

    for (const auto &expression : expressions) {
        buffer << " ";
        expression->accept(*this);
    }

    buffer << ")";
}
