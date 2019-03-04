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

Function::Function(Token_ptr name, vector<Token_ptr> &&parameters, vector<Statement_ptr> &&body)
    : name{move(name)}, parameters{move(parameters)}, body{move(body)} {}

Return::Return(Token_ptr keyword, Expression_ptr value)
    : keyword{move(keyword)}, value{move(value)} {}


// Factory Functions

std::shared_ptr<ExpressionStatement> ExpressionStatement::New(Expression_ptr expression) {
    return make_shared<ExpressionStatement>(move(expression));
}

std::shared_ptr<Print> Print::New(Expression_ptr expression) {
    return make_shared<Print>(move(expression));
}

std::shared_ptr<Block> Block::New(std::vector<Statement_ptr> &&statements) {
    return make_shared<Block>(move(statements));
}

std::shared_ptr<Var> Var::New(Token_ptr name, Expression_ptr initializer) {
    return make_shared<Var>(move(name), move(initializer));
}

std::shared_ptr<If> If::New(Expression_ptr condition, Statement_ptr thenBranch, Statement_ptr elseBranch) {
    return make_shared<If>(move(condition), move(thenBranch), move(elseBranch));
}

std::shared_ptr<While> While::New(Expression_ptr condition, Statement_ptr body) {
    return make_shared<While>(move(condition), move(body));
}

std::shared_ptr<Function> Function::New(Token_ptr name, std::vector<Token_ptr> &&parameters, std::vector<Statement_ptr> &&body) {
    return make_shared<Function>(move(name), move(parameters), move(body));
}

std::shared_ptr<Return> Return::New(Token_ptr keyword, Expression_ptr value) {
    return make_shared<Return>(move(keyword), move(value));
}


// Visitor

void ExpressionStatement::accept(StatementVisitor &v) {
    v.visit(*this);
}

void Print::accept(StatementVisitor &v) {
    v.visit(*this);
}

void Block::accept(StatementVisitor &v) {
    v.visit(*this);
}

void Var::accept(StatementVisitor &v) {
    v.visit(*this);
}

void If::accept(StatementVisitor &v) {
    v.visit(*this);
}

void While::accept(StatementVisitor &v) {
    v.visit(*this);
}

void Function::accept(StatementVisitor &v) {
    v.visit(*this);
}

void Return::accept(StatementVisitor &v) {
    v.visit(*this);
}