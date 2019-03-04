//
// Created by Lucas Wolf on 2019-03-03.
//

#ifndef LOX_INTERPRETER_RESOLVER_H
#define LOX_INTERPRETER_RESOLVER_H

#include <data/expression.h>
#include <data/statement.h>
#include <interpreter/Interpreter.h>
#include <error.h>

#include <vector>
#include <unordered_map>

void resolve(const std::vector<Statement_ptr> &statements, Interpreter &interpreter);

class Resolver : public ExpressionVisitor, StatementVisitor {
public:
    explicit Resolver(Interpreter &interpreter);

    void resolve(const std::vector<Statement_ptr> &statements);
    void resolve(Statement &statement);
    void resolve(Expression &expression);

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

    enum class FunctionType {
        NONE, FUNCTION
    };


private:

    void beginScope();
    void endScope();

    void declare(const Token &name);
    void define(const Token &name);

    void resolveLocal(Expression &expression, const Token &name);
    void resolveFunction(const Function &function, const FunctionType &type);

    Interpreter &interpreter;

    // each element in the stack is a map representing a single block scope; the key is the name of an object in the
    // scope, its associated value indicates whether it has been initialized completely (yet).
    std::vector<std::unordered_map<std::string, bool>> scopes;

    // Indicator that tracks whether we are currently inside a function or not
    Resolver::FunctionType currentFunction = Resolver::FunctionType::NONE;
};

struct ResolverError : public LoxError {
    Token_ptr token;
    std::string message;

    ResolverError(Token_ptr token, std::string message);
    std::string what() const noexcept override;
};


#endif //LOX_INTERPRETER_RESOLVER_H
