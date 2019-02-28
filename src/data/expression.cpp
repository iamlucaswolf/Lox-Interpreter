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
    
Unary::Unary(Token_ptr token, Expression_ptr operand)
    : token{move(token)}, operand{move(operand)} {}
    
Variable::Variable(Token_ptr name)
    : name{move(name)} {}

//Assign::Assign(const Token &name, Expression_ptr value)
//    : name{name}, value{move(value)} {}

Assign::Assign(Token_ptr name, Expression_ptr value)
    : name{move(name)}, value{move(value)} {}

// Visitor

void Binary::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}

void Grouping::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}

void Literal::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}

void Unary::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}

void Variable::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}

void Assign::accept(ExpressionVisitor &v) const {
    v.visit(*this);
}