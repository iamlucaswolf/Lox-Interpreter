//
// Created by Lucas Wolf on 2019-03-03.
//

#include "Resolver.h"

using namespace std;

using FunctionType = Resolver::FunctionType;

void resolve(const std::vector<Statement_ptr> &statements, Interpreter &interpreter) {
    Resolver resolver{interpreter};
    resolver.resolve(statements);
}

Resolver::Resolver(Interpreter &interpreter)
    : interpreter{interpreter} {}

void Resolver::resolve(const vector<Statement_ptr> &statements) {
    for (const auto &statement : statements) {
        resolve(*statement);
    }
}

void Resolver::resolve(Statement &statement) {
    statement.accept(*this);
}

void Resolver::resolve(Expression &expression) {
    expression.accept(*this);
}

// Statement Visitor

void Resolver::visit(ExpressionStatement &statement) {
    resolve(*statement.expression);
}

void Resolver::visit(Print &statement) {
    resolve(*statement.expression);
}

void Resolver::visit(Var &statement) {
    declare(*statement.name);

    if (statement.initializer) {
        resolve(*statement.initializer);
    }

    define(*statement.name);
}

void Resolver::visit(If &statement) {
    resolve(*statement.condition);
    resolve(*statement.thenBranch);

    if (statement.elseBranch) {
        resolve(*statement.elseBranch);
    }
}

void Resolver::visit(Block &statement) {
    beginScope();
    resolve(statement.statements);
    endScope();
}

void Resolver::visit(While &statement) {
    resolve(*statement.condition);
    resolve(*statement.body);
}

void Resolver::visit(Function &statement) {
    declare(*statement.name);
    define(*statement.name);

    resolveFunction(statement, FunctionType::FUNCTION);
}

void Resolver::visit(Return &statement) {
    if (currentFunction == FunctionType::NONE) {
        throw ResolverError(statement.keyword, "Cannot return from top-level code.");
    }

    if (statement.value) {
        resolve(*statement.value);
    }
}

// Expression Visitor

void Resolver::visit(Binary &expression) {
    resolve(*expression.left);
    resolve(*expression.right);
}

void Resolver::visit(Grouping &expression) {
    resolve(*expression.content);
}

void Resolver::visit(Literal &expression) {
    // Nothing to do.
}

void Resolver::visit(Logical &expression) {
    resolve(*expression.left);
    resolve(*expression.right);
}

void Resolver::visit(Unary &expression) {
    resolve(*expression.operand);
}

void Resolver::visit(Variable &expression) {

    // if we are currently in some local scope and the variable is declared inside it but not defined, throw an error
    if (not scopes.empty()) {
        auto &scope = scopes.back();
        auto found = scope.find(expression.name->lexeme);

        if (found != scope.end() and not found->second) {
            throw ResolverError(expression.name, "Cannot read local variable in its own initializer");
        }

    }

    resolveLocal(expression, *expression.name);
}

void Resolver::visit(Assign &expression) {
    resolve(*expression.value);
    resolveLocal(expression, *expression.name);
}

void Resolver::visit(Call &expression) {
    resolve(*expression.callee);

    for (const auto &argument : expression.arguments) {
        resolve(*argument);
    }
}

// Helper methods

void Resolver::beginScope() {
    scopes.emplace_back();
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token &name) {
    if (scopes.empty()) {
        return;
    }

    auto &scope = scopes.back();

    if(scope.find(name.lexeme) != scope.end()) {
        // TODO ownership!
        throw ResolverError(make_shared<Token>(name), "Variable with this name already declared in this scope.");
    }

    scope[name.lexeme] = false;
}


void Resolver::define(const Token &name) {
    if (scopes.empty()) {
        return;
    }

    auto &scope = scopes.back();
    scope[name.lexeme] = true;
}


void Resolver::resolveLocal(Expression &expression, const Token &name) {
    // stack::size() returns size_t which might underflow
    int size = static_cast<int>(scopes.size());

    // Start at innermost scope, working outwards
    for (auto i = size - 1; i >= 0; i--) {
        auto &scope = scopes[i];

        // if name can be resolved in the current scope
        if (scope.find(name.lexeme) != scope.end()) {
            interpreter.resolve(expression, size - 1 - i);
            return;
        }
    }

    // Not found. Assume it's global
}

void Resolver::resolveFunction(const Function &function, const FunctionType &type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();

    for (const auto &parameter : function.parameters) {
        declare(*parameter);
        define(*parameter);
    }

    resolve(function.body);
    endScope();

    currentFunction = enclosingFunction;
}


// ResolverError

ResolverError::ResolverError(Token_ptr token, string message)
    : token{move(token)}, message{move(message)} {}

string ResolverError::what() const noexcept {
    return report(token->line, "", message);
}