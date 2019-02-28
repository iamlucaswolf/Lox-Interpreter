//
// Created by Lucas Wolf on 2019-01-25.
//

#ifndef LOX_INTERPRETER_AST_H
#define LOX_INTERPRETER_AST_H

#include "token.h"

// Forward declarations
struct Expression;
struct Binary;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;
struct Assign;


using Expression_ptr = std::unique_ptr<Expression>;


class ExpressionVisitor {

public:
    virtual void visit(const Binary &expression) = 0;
    virtual void visit(const Grouping &expression) = 0;
    virtual void visit(const Literal &expression) = 0;
    virtual void visit(const Logical &expression) = 0;
    virtual void visit(const Unary &expression) = 0;
    virtual void visit(const Variable &expression) = 0;
    virtual void visit(const Assign &expression) = 0;
};


struct Expression {
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor &v) const = 0;
};


struct Binary : public Expression {
    Expression_ptr left;
    Token_ptr token;
    Expression_ptr right;

    Binary(Expression_ptr left, Token_ptr token, Expression_ptr right);
    void accept(ExpressionVisitor &v) const override;
};


struct Grouping : public Expression {
    Expression_ptr content;

    explicit Grouping(Expression_ptr content);
    void accept(ExpressionVisitor &v) const override;
};


struct Literal : public Expression {
    Token_ptr token;

    explicit Literal(Token_ptr token);
    void accept(ExpressionVisitor &v) const override;
};


struct Logical : public Expression {
    Expression_ptr left;
    Token_ptr token;
    Expression_ptr right;

    Logical(Expression_ptr left, Token_ptr token, Expression_ptr right);
    void accept(ExpressionVisitor &v) const override;
};


struct Unary : public Expression {
    Token_ptr token;
    Expression_ptr operand;

    Unary(Token_ptr token, Expression_ptr operand);
    void accept(ExpressionVisitor &v) const override;
};


struct Variable : public Expression {
    Token_ptr name;

    explicit Variable(Token_ptr name);
    void accept(ExpressionVisitor &v) const override;
};


struct Assign : public Expression {
    // TODO possible to own Variable and use the nested name pointer?
    Token_ptr name;
    Expression_ptr  value;

    Assign(Token_ptr name, Expression_ptr value);
    void accept(ExpressionVisitor &v) const override;
};


#endif //LOX_INTERPRETER_AST_H
