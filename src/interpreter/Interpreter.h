//
// Created by Lucas Wolf on 2019-02-22.
//

#ifndef LOX_INTERPRETER_INTERPRETER_H
#define LOX_INTERPRETER_INTERPRETER_H

#include "data/ast.h"
#include "LoxObject.h"

#include <stack>
#include <error.h>

using Comparison = std::function<bool(double, double)>;
using Arithmetic = std::function<double(double, double)>;

/*
 * Executes Lox statements given as an Abstract Syntax Tree (AST)
 */
class Interpreter : public Visitor {

private:
    // Cache for intermediary results during expression evaluations
    std::unique_ptr<LoxObject> result;

    void evaluate(const Expression &expression);

    // Helper functions
    void comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token);
    void arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token);

public:

    // Evaluate a piece of Lox code (currently, determine the value of an expression and write it to stdout)
    void interpret(const Expression &expression);

    // Member functions for Expression visitor interface
    void visit(const Binary &expression) override;
    void visit(const Grouping &expression) override;
    void visit(const Literal &expression) override;
    void visit(const Unary &expression) override;
};


/*
 * Signals the occurence of an Error during execution.
 */
struct RuntimeError : LoxError {

    const Token &token;
    const std::string message;

    RuntimeError(const Token &token, std::string message);

    std::string what() const noexcept;
};


#endif //LOX_INTERPRETER_INTERPRETER_H
