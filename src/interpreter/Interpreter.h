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
    void evaluate(const Expression &expression);
    void execute(const Statement &statement);
    void executeBlock(const std::vector<Statement_ptr> &statements, Environment_ptr environment);

    // Member functions for Statement visitor interface
    void visit(const ExpressionStatement &statement) override;
    void visit(const Print &statement) override;
    void visit(const Block &statement) override;
    void visit(const Var &statement) override;
    void visit(const If &statement) override;
    void visit(const While &statement) override;
    void visit(const Function &statement) override;
    void visit(const Return &statement) override;

    // Member functions for Expression visitor interface
    void visit(const Binary &expression) override;
    void visit(const Grouping &expression) override;
    void visit(const Literal &expression) override;
    void visit(const Logical &expression) override;
    void visit(const Unary &expression) override;
    void visit(const Variable &expression) override;
    void visit(const Assign &expression) override;
    void visit(const Call &expression) override;

private:
    // intermediate result of expression evaluation
    Object_ptr temporary = nullptr;

    // currently active environment
    Environment_ptr environment;

    // Helper functions
    void comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token);
    void arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token);
};


struct ReturnValue {
    Object_ptr value;

    explicit ReturnValue(Object_ptr value = nullptr) : value{move(value)} {}
};

#endif //LOX_INTERPRETER_INTERPRETER_H
