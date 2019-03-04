//
// Created by Lucas Wolf on 2019-02-26.
//

#include "expression.h"

using namespace std;

// Constructors

Binary::Binary(Expression_ptr left, Token_ptr token, Expression_ptr right)
    : left{move(left)}, token{move(token)}, right{move(right)} {}
        
Grouping::Grouping(Expression_ptr content)
    : content{move(content)} {}
    
Literal::Literal(Token_ptr token) 
    : token{move(token)} {}

Logical::Logical(Expression_ptr left, Token_ptr token, Expression_ptr right)
        : left{move(left)}, token{move(token)}, right{move(right)} {}
    
Unary::Unary(Token_ptr token, Expression_ptr operand)
    : token{move(token)}, operand{move(operand)} {}
    
Variable::Variable(Token_ptr name)
    : name{move(name)} {}

Assign::Assign(Token_ptr name, Expression_ptr value)
    : name{move(name)}, value{move(value)} {}

Call::Call(Expression_ptr callee, Token_ptr paren, std::vector<Expression_ptr> &&arguments)
    : callee{move(callee)}, paren{move(paren)}, arguments{move(arguments)} {}


// Factory functions

std::shared_ptr<Binary> Binary::New(Expression_ptr left, Token_ptr token, Expression_ptr right) {
    return make_shared<Binary>(move(left), move(token), move(right));
}

std::shared_ptr<Grouping> Grouping::New(Expression_ptr content) {
    return make_shared<Grouping>(move(content));
}

std::shared_ptr<Literal> Literal::New(Token_ptr token) {
    return make_shared<Literal>(move(token));
}

std::shared_ptr<Logical> Logical::New(Expression_ptr left, Token_ptr token, Expression_ptr right) {
    return make_shared<Logical>(move(left), move(token), move(right));
}

std::shared_ptr<Unary> Unary::New(Token_ptr token, Expression_ptr operand) {
    return make_shared<Unary>(move(token), move(operand));
}

std::shared_ptr<Variable> Variable::New(Token_ptr name) {
    return make_shared<Variable>(move(name));
}

std::shared_ptr<Assign> Assign::New(Token_ptr name, Expression_ptr value) {
    return make_shared<Assign>(move(name), move(value));
}

std::shared_ptr<Call> Call::New(Expression_ptr callee, Token_ptr paren, std::vector<Expression_ptr> &&arguments) {
    return make_shared<Call>(move(callee), move(paren), move(arguments));
}


// Visitor

void Binary::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Grouping::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Literal::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Logical::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Unary::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Variable::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Assign::accept(ExpressionVisitor &v) {
    v.visit(*this);
}

void Call::accept(ExpressionVisitor &v) {
    v.visit(*this);
}