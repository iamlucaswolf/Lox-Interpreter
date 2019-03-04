//
// Created by Lucas Wolf on 2019-02-13.
//

#ifndef LOX_INTERPRETER_AST_TOOLS_H
#define LOX_INTERPRETER_AST_TOOLS_H

#include <string>
#include <sstream>
#include <initializer_list>
#include "data/expression.h"

class Printer : public ExpressionVisitor {

    std::stringstream buffer;

    void parenthesized(std::string name, std::initializer_list<Expression*> expressions);

public:

    std::string print(::Expression &expression);

    void visit(Binary &expression) override;
    void visit(Grouping &expression) override;
    void visit(Literal &expression) override;
    void visit(Unary &expression) override;

};

#endif //LOX_INTERPRETER_AST_TOOLS_H
