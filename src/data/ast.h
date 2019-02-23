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
    std::shared_ptr<Token> token;
    std::unique_ptr<Expression> right;

    Binary(std::unique_ptr<Expression> left, std::shared_ptr<Token> token, std::unique_ptr<Expression> right) :
        left{ std::move(left) }, token{ std::move(token) }, right{ std::move(right) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Grouping : public Expression {
    std::unique_ptr<Expression> content;

    explicit Grouping(std::unique_ptr<Expression> content) : content{ std::move(content) } {}
    
    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Literal : public Expression {
    std::shared_ptr<Token> token;

    explicit Literal(std::shared_ptr<Token> token) : token{ std::move(token) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};

struct Unary : public Expression {
    std::shared_ptr<Token> token;
    std::unique_ptr<Expression> operand;

    Unary(std::shared_ptr<Token> token, std::unique_ptr<Expression> operand) :
        token{ std::move(token) }, operand{ std::move(operand) } {}

    void accept(Visitor &v) const override {
        v.visit(*this);
    }
};


#endif //LOX_INTERPRETER_AST_H
