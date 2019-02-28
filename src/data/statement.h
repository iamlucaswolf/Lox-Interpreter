//
// Created by Lucas Wolf on 2019-02-23.
//

#ifndef LOX_INTERPRETER_STATEMENT_H
#define LOX_INTERPRETER_STATEMENT_H

#include <memory>
#include <vector>
#include "expression.h"


// Forward declarations
struct Statement;
struct ExpressionStatement;
struct Print;
struct Block;
struct Var;
struct If;
struct While;


using Statement_ptr = std::unique_ptr<Statement>;


class StatementVisitor {

public:
    virtual void visit(const ExpressionStatement &statement) = 0;
    virtual void visit(const Print &statement) = 0;
    virtual void visit(const Block &statement) = 0;
    virtual void visit(const Var &statement) = 0;
    virtual void visit(const If &statement) = 0;
    virtual void visit(const While &statement) = 0;
};


struct Statement{
    virtual ~Statement() = default;
    virtual void accept(StatementVisitor &v) const = 0;
};


struct ExpressionStatement : public Statement {
    Expression_ptr expression;

    explicit ExpressionStatement(Expression_ptr expression);
    void accept(StatementVisitor &v) const override;
};


struct Print : public Statement {
    Expression_ptr expression;

    explicit Print(Expression_ptr expression);
    void accept(StatementVisitor &v) const override;
};


struct Block : public Statement {
    std::vector<Statement_ptr> statements;

    explicit Block(std::vector<Statement_ptr> &&statements);
    void accept(StatementVisitor &v) const override;
};


struct Var : public Statement {
    Token_ptr name;
    Expression_ptr initializer;

    explicit Var(Token_ptr name, Expression_ptr initializer);
    void accept(StatementVisitor &v) const override;
};


struct If : public Statement {
    Expression_ptr condition;
    Statement_ptr thenBranch;
    Statement_ptr elseBranch;

    explicit If(Expression_ptr condition, Statement_ptr thenBranch, Statement_ptr elseBranch);
    void accept(StatementVisitor &v) const override;
};


struct While : public Statement {
    Expression_ptr condition;
    Statement_ptr body;

    explicit While(Expression_ptr condition, Statement_ptr body);
    void accept(StatementVisitor &v) const override;
};

#endif //LOX_INTERPRETER_STATEMENT_H
