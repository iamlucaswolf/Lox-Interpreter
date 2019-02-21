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

// TODO is shared_ptr<Token> really a good idea? Shouldn't we rather move from the Token vector?

struct Expression {
    virtual ~Expression() = default;
    virtual void accept(Visitor &v) const = 0;
};

struct Binary : public Expression {
    std::unique_ptr<Expression> left;
    std::shared_ptr<Token> infix;
    std::unique_ptr<Expression> right;

    Binary(std::unique_ptr<Expression> left, std::shared_ptr<Token> infix, std::unique_ptr<Expression> right) :
        left{ std::move(left) }, infix{ std::move(infix) }, right{ std::move(right) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Grouping : public Expression {
    std::unique_ptr<Expression> expression;

    explicit Grouping(std::unique_ptr<Expression> expression) : expression{ std::move(expression) } {}
    
    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Literal : public Expression {
    std::shared_ptr<Token> literal;

    explicit Literal(std::shared_ptr<Token> literal) : literal{ std::move(literal) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Unary : public Expression {
    std::shared_ptr<Token> prefix;
    std::unique_ptr<Expression> expression;

    Unary(std::shared_ptr<Token> prefix, std::unique_ptr<Expression> expression) :
        prefix{ std::move(prefix) }, expression{ std::move(expression) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};


#endif //LOX_INTERPRETER_AST_H
