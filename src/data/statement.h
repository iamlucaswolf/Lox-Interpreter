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
struct Function;
struct Return;


using Statement_ptr = std::shared_ptr<Statement>;


class StatementVisitor {

public:
    virtual void visit(const ExpressionStatement &statement) = 0;
    virtual void visit(const Print &statement) = 0;
    virtual void visit(const Block &statement) = 0;
    virtual void visit(const Var &statement) = 0;
    virtual void visit(const If &statement) = 0;
    virtual void visit(const While &statement) = 0;
    virtual void visit(const Function &statement) = 0;
    virtual void visit(const Return &statement) = 0;
};


struct Statement{
    virtual ~Statement() = default;
    virtual void accept(StatementVisitor &v) const = 0;
};


struct ExpressionStatement : public Statement {
    Expression_ptr expression;

    explicit ExpressionStatement(Expression_ptr expression);
    static std::shared_ptr<ExpressionStatement> New(Expression_ptr expression);

    void accept(StatementVisitor &v) const override;
};


struct Print : public Statement {
    Expression_ptr expression;

    explicit Print(Expression_ptr expression);
    static std::shared_ptr<Print> New(Expression_ptr expression);

    void accept(StatementVisitor &v) const override;
};


struct Block : public Statement {
    std::vector<Statement_ptr> statements;

    explicit Block(std::vector<Statement_ptr> &&statements);
    static std::shared_ptr<Block> New(std::vector<Statement_ptr> &&statements);

    void accept(StatementVisitor &v) const override;
};


struct Var : public Statement {
    Token_ptr name;
    Expression_ptr initializer;

    explicit Var(Token_ptr name, Expression_ptr initializer);
    static std::shared_ptr<Var> New(Token_ptr name, Expression_ptr initializer);

    void accept(StatementVisitor &v) const override;
};


struct If : public Statement {
    Expression_ptr condition;
    Statement_ptr thenBranch;
    Statement_ptr elseBranch;

    explicit If(Expression_ptr condition, Statement_ptr thenBranch, Statement_ptr elseBranch);
    static std::shared_ptr<If> New(Expression_ptr condition, Statement_ptr thenBranch, Statement_ptr elseBranch);

    void accept(StatementVisitor &v) const override;
};


struct While : public Statement {
    Expression_ptr condition;
    Statement_ptr body;

    explicit While(Expression_ptr condition, Statement_ptr body);
    static std::shared_ptr<While> New(Expression_ptr condition, Statement_ptr body);

    void accept(StatementVisitor &v) const override;
};


struct Function : public Statement {
    Token_ptr name;
    std::vector<Token_ptr> parameters;
    std::vector<Statement_ptr> body;

    explicit Function(Token_ptr name, std::vector<Token_ptr> &&parameters, std::vector<Statement_ptr> &&body);
    static std::shared_ptr<Function> New(Token_ptr name, std::vector<Token_ptr> &&parameters, std::vector<Statement_ptr> &&body);

    void accept(StatementVisitor &v) const override;
};


struct Return : public Statement {
    Token_ptr keyword;
    Expression_ptr value;

    explicit Return(Token_ptr keyword, Expression_ptr value);
    static std::shared_ptr<Return> New(Token_ptr keyword, Expression_ptr value);

    void accept(StatementVisitor &v) const override;
};
#endif //LOX_INTERPRETER_STATEMENT_H
