//
// Created by Lucas Wolf on 2019-01-25.
//

#ifndef LOX_INTERPRETER_AST_H
#define LOX_INTERPRETER_AST_H

#include "token.h"

class Visitor;

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

// Can't return a value from visitor, because C++ doesn't allow generics on virtual methods. This would break in
// the expression struct when defining something like template <typename T> T accept(Visitor<T> &v) = 0;
// Instead, we'll leave it to the internal state of the visitor
class Visitor {

public:
    virtual void visit(const Binary &expression) = 0;
    virtual void visit(const Grouping &expression) = 0;
    virtual void visit(const Literal &expression) = 0;
    virtual void visit(const Unary &expression) = 0;
};


struct Expression {
    virtual void accept(Visitor &v) const = 0;
};

// TODO should expressions own their subexpressions? (probably not, since there are no deconstructors to be called anyway?)
// TODO caveat: member references can never be rebound or null... -> maybe use unique_ptr<> members and let the constructor
// TODO std::move() r-value references?
struct Binary : public Expression {
    const Expression &left;
    const Token &infix;
    const Expression &right;

    Binary(const Expression &left, const Token &infix, const Expression &right)
        : left{left}, right{right}, infix{infix} {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Grouping : public Expression {
    const Expression &expression;

    explicit Grouping(const Expression &expression) : expression { expression } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Literal : public Expression {
    const Token &literal;

    explicit Literal(const Token &literal) : literal{literal} {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Unary : public Expression {
    const Token &prefix;
    const Expression &expression;

    Unary(const Token &prefix, const Expression &expression) : prefix{prefix}, expression{expression} {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};


#endif //LOX_INTERPRETER_AST_H
