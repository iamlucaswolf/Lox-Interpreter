//
// Created by Lucas Wolf on 2019-02-23.
//

#include "statement.h"

using namespace std;

// Constructors

ExpressionStatement::ExpressionStatement(Expression_ptr expression)
    : expression{move(expression)} {}

Print::Print(Expression_ptr expression)
    : expression{move(expression)} {}

Block::Block(std::vector<Statement_ptr> &&statements)
    : statements{move(statements)} {}

Var::Var(Token_ptr name, Expression_ptr initializer)
    : name{move(name)}, initializer{move(initializer)} {}


// Visitor

void ExpressionStatement::accept(StatementVisitor &v) const {
    v.visit(*this);
}

void Print::accept(StatementVisitor &v) const {
    v.visit(*this);
}

void Block::accept(StatementVisitor &v) const {
    v.visit(*this);
}

void Var::accept(StatementVisitor &v) const {
    v.visit(*this);
}