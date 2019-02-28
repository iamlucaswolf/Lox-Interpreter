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

If::If(Expression_ptr condition, Statement_ptr thenBranch, Statement_ptr elseBranch)
    : condition{move(condition)}, thenBranch{move(thenBranch)}, elseBranch{move(elseBranch)} {}

While::While(Expression_ptr condition, Statement_ptr body)
    : condition{move(condition)}, body{move(body)} {}


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

void If::accept(StatementVisitor &v) const {
    v.visit(*this);
}

void While::accept(StatementVisitor &v) const {
    v.visit(*this);
}