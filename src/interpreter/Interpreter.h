//
// Created by Lucas Wolf on 2019-02-22.
//

#ifndef LOX_INTERPRETER_INTERPRETER_H
#define LOX_INTERPRETER_INTERPRETER_H

#include <vector>
#include <memory>
#include <error.h>

#include "data/expression.h"
#include "data/statement.h"
#include "LoxObject.h"
#include "Environment.h"

using Comparison = std::function<bool(double, double)>;
using Arithmetic = std::function<double(double, double)>;

/*
 * Executes Lox statements given as an Abstract Syntax Tree (AST)
 */
class Interpreter : public ExpressionVisitor, StatementVisitor {

public:
    Environment_ptr globals;

    Interpreter();

    // Interpret Lox code
    void interpret(const std::vector<Statement_ptr> &statements);
    void evaluate(Expression &expression);
    void execute(Statement &statement);
    void executeBlock(const std::vector<Statement_ptr> &statements, Environment_ptr environment);
    void resolve(Expression &expression, int depth);

    // Member functions for Statement visitor interface
    void visit(ExpressionStatement &statement) override;
    void visit(Print &statement) override;
    void visit(Block &statement) override;
    void visit(Var &statement) override;
    void visit(If &statement) override;
    void visit(While &statement) override;
    void visit(Function &statement) override;
    void visit(Return &statement) override;

    // Member functions for Expression visitor interface
    void visit(Binary &expression) override;
    void visit(Grouping &expression) override;
    void visit(Literal &expression) override;
    void visit(Logical &expression) override;
    void visit(Unary &expression) override;
    void visit(Variable &expression) override;
    void visit(Assign &expression) override;
    void visit(Call &expression) override;

private:
    // intermediate result of expression evaluation
    Object_ptr temporary = nullptr;

    // currently active environment
    Environment_ptr environment;

    // side table for resolutions
    std::unordered_map<Expression*, int> locals;

    Object_ptr lookUpVariable(const Token &name, Expression &expression);

    // Helper functions
    void comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token);
    void arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token);
};


struct ReturnValue {
    Object_ptr value;

    explicit ReturnValue(Object_ptr value = nullptr) : value{move(value)} {}
};

#endif //LOX_INTERPRETER_INTERPRETER_H
