//
// Created by Lucas Wolf on 2019-01-25.
//

#ifndef LOX_INTERPRETER_AST_H
#define LOX_INTERPRETER_AST_H

#include "token.h"

#include <vector>

// Forward declarations
struct Expression;
struct Binary;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;
struct Assign;
struct Call;


using Expression_ptr = std::shared_ptr<Expression>;


/*
 * Visitor interface for Expressions
 */
class ExpressionVisitor {

public:
    virtual void visit(Binary &expression) = 0;
    virtual void visit(Grouping &expression) = 0;
    virtual void visit(Literal &expression) = 0;
    virtual void visit(Logical &expression) = 0;
    virtual void visit(Unary &expression) = 0;
    virtual void visit(Variable &expression) = 0;
    virtual void visit(Assign &expression) = 0;
    virtual void visit(Call &expression) = 0;
};

// Abstract base struct for Expressions
struct Expression {
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor &v) = 0;
};


// A binary arithmetic operation <left op right>
struct Binary : public Expression {
    Expression_ptr left;
    Token_ptr token;
    Expression_ptr right;

    explicit Binary(Expression_ptr left, Token_ptr token, Expression_ptr right);
    static std::shared_ptr<Binary> New(Expression_ptr left, Token_ptr token, Expression_ptr right);

    void accept(ExpressionVisitor &v) override;
};


struct Grouping : public Expression {
    Expression_ptr content;

    explicit Grouping(Expression_ptr content);
    static std::shared_ptr<Grouping> New(Expression_ptr content);

    void accept(ExpressionVisitor &v) override;
};


struct Literal : public Expression {
    Token_ptr token;

    explicit Literal(Token_ptr token);
    static std::shared_ptr<Literal> New(Token_ptr token);

    void accept(ExpressionVisitor &v) override;
};


struct Logical : public Expression {
    Expression_ptr left;
    Token_ptr token;
    Expression_ptr right;

    explicit Logical(Expression_ptr left, Token_ptr token, Expression_ptr right);
    static std::shared_ptr<Logical> New(Expression_ptr left, Token_ptr token, Expression_ptr right);

    void accept(ExpressionVisitor &v) override;
};


struct Unary : public Expression {
    Token_ptr token;
    Expression_ptr operand;

    explicit Unary(Token_ptr token, Expression_ptr operand);
    static std::shared_ptr<Unary> New(Token_ptr token, Expression_ptr operand);

    void accept(ExpressionVisitor &v) override;
};


struct Variable : public Expression {
    Token_ptr name;

    explicit Variable(Token_ptr name);
    static std::shared_ptr<Variable> New(Token_ptr name);

    void accept(ExpressionVisitor &v) override;
};


struct Assign : public Expression {
    Token_ptr name;
    Expression_ptr  value;

    Assign(Token_ptr name, Expression_ptr value);
    static std::shared_ptr<Assign> New(Token_ptr name, Expression_ptr value);

    void accept(ExpressionVisitor &v) override;
};


struct Call : public Expression {
    Expression_ptr callee;
    Token_ptr paren; //closing!
    std::vector<Expression_ptr> arguments;

    Call(Expression_ptr callee, Token_ptr paren, std::vector<Expression_ptr> &&arguments);
    static std::shared_ptr<Call> New(Expression_ptr callee, Token_ptr paren, std::vector<Expression_ptr> &&arguments);

    void accept(ExpressionVisitor &v) override;
};


#endif //LOX_INTERPRETER_AST_H
