//
// Created by Lucas Wolf on 2019-02-22.
//

#ifndef LOX_INTERPRETER_INTERPRETER_H
#define LOX_INTERPRETER_INTERPRETER_H

#include <vector>
#include <memory>

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
    // Interpret Lox code
    void interpret(const std::vector<std::unique_ptr<Statement>> &statements);
    void evaluate(const Expression &expression);
    void execute(const Statement &statement);

    // Member functions for Statement visitor interface
    void visit(const ExpressionStatement &statement) override;
    void visit(const Print &statement) override;
    void visit(const Block &statement) override;
    void visit(const Var &statement) override;

    // Member functions for Expression visitor interface
    void visit(const Binary &expression) override;
    void visit(const Grouping &expression) override;
    void visit(const Literal &expression) override;
    void visit(const Unary &expression) override;
    void visit(const Variable &expression) override;
    void visit(const Assign &expression) override;

private:
    std::shared_ptr<LoxObject> temporary = nullptr;
    std::unique_ptr<Environment> environment = std::make_unique<Environment>();

    // Helper functions
    void comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token);
    void arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token);
};

#endif //LOX_INTERPRETER_INTERPRETER_H
