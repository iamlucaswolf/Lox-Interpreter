//
// Created by Lucas Wolf on 2019-02-22.
//

#include "Interpreter.h"
#include "RuntimeError.h"
#include "LoxObject.h"
#include "Callable.h"

#include <vector>
#include <chrono>

using namespace std;

Interpreter::Interpreter() : globals{move(make_unique<Environment>())} {

    environment = globals;

    // Define native functions

    auto clock = make_shared<Native>(0, [](Interpreter &interpreter, vector<Object_ptr> &arguments){
        auto now = chrono::system_clock::now();
        auto since = chrono::duration_cast<chrono::seconds>(now.time_since_epoch());

        return Number::New(since.count());
    });


    globals->define("clock", clock);
}

void Interpreter::interpret(const vector<Statement_ptr> &statements) {
    for (const auto &statement : statements) {
        execute(*statement);
    }
}

void Interpreter::execute(const Statement &statement) {
    statement.accept(*this);
}

void Interpreter::visit(const ExpressionStatement &statement) {
    evaluate(*statement.expression);
}

void Interpreter::visit(const Block &statement) {
    executeBlock(statement.statements, make_shared<Environment>(this->environment));
}

void Interpreter::executeBlock(const std::vector<Statement_ptr> &statements, Environment_ptr environment) {
    // retain current environment
    auto previous = this->environment;

    try {
        // switch to new environment
        this->environment = move(environment);

        // execute block statements in new environment
        for (const auto &s : statements) {
            execute(*s);
        }

    } catch(...) {
        // Restore the old environment
        this->environment = previous;
        throw;
    }

    // TODO better, finally-like solution?

    // Restore the old environment
    this->environment = previous;
}


void Interpreter::visit(const Print &statement) {
    evaluate(*statement.expression);
    cout << *temporary << "\n";
}

void Interpreter::visit(const Var &statement) {

    if (statement.initializer) {
        evaluate(*statement.initializer);
    } else {
        temporary = Nil::New();
    }

    // copies shared_ptr
    environment->define(statement.name->lexeme, temporary);
}

void Interpreter::visit(const If &statement) {
    evaluate(*statement.condition);

    if (temporary->isTruthy()) {
        execute(*statement.thenBranch);
    } else if (statement.elseBranch) {
        execute(*statement.elseBranch);
    }
}

void Interpreter::visit(const While &statement) {
    evaluate(*statement.condition);

    while (temporary->isTruthy()) {
        execute(*statement.body);
        evaluate(*statement.condition);
    }
}


void Interpreter::visit(const Function &statement) {
    // copying the vector should invoke copy constructors of its elements (ie. Token_ptr = shared_ptr<Token>), thus
    // creating new owning pointers that ensure that these things live beyond parsing
    vector<Token_ptr> parameters = statement.parameters;
    vector<Statement_ptr> body = statement.body;

    auto function = Function::New(statement.name, move(parameters), move(body));
    environment->define(statement.name->lexeme, FunctionObject::New(move(function), environment));
}


void Interpreter::visit(const Return &statement) {

    if (statement.value) {
        evaluate(*statement.value);
        throw ReturnValue {move(temporary)};
    }

    throw ReturnValue{};
}


void Interpreter::visit(const Call &expression) {
    evaluate(*expression.callee);
    auto callee = temporary;

    vector<Object_ptr> arguments;

    // evaluate arguments to call
    for (const auto &argument : expression.arguments) {
        evaluate(*argument);
        arguments.push_back(temporary);
    }

    auto callable = dynamic_cast<Callable*>(callee.get());

    if (not callable) {
        throw RuntimeError(*expression.paren, "Can only call functions and classes.");
    }

    if (arguments.size() != callable->arity()) {
        throw RuntimeError(
            *expression.paren,
            "Expected " + to_string(callable->arity()) + " arguments but got " + to_string(arguments.size()) + "."
        );
    }

    temporary = callable->call(*this, arguments);
}

void Interpreter::visit(const Unary &expression) {
    evaluate(*expression.operand);

    const Token &token = *expression.token;

    switch (token.type) {

        // Unary minus (-)
        case TokenType::MINUS: {
            auto number = dynamic_cast<Number*>(temporary.get());

            if (not number) {
                throw RuntimeError(token, "Operand of unary minus (-) must be of type Number.");
            }

            number->value = -(number->value);
            break;
        }

        // Logical negation (!)
        case TokenType::BANG: {
            temporary = Boolean::New(temporary->isTruthy());
            break;
        }

        default: ; // Unreachable
    }
}

void Interpreter::visit(const Binary &expression) {

    // Evaluate left and right-hand operands

    evaluate(*expression.left);
    auto left = temporary;

    evaluate(*expression.right);
    auto right = temporary;

    const Token &token = *expression.token;

    switch(token.type) {

        case TokenType::MINUS: {
            arithmetic(*left, *right, [](auto left, auto right) { return left - right;}, token);
            break;
        }

        case TokenType::SLASH: {
            arithmetic(*left, *right, [](auto left, auto right) { return left / right;}, token);
            break;
        }

        case TokenType::STAR: {
            arithmetic(*left, *right, [](auto left, auto right) { return left * right;}, token);
            break;
        }

        case TokenType::PLUS: {
            if (dynamic_cast<const Number*>(left.get())) {
                arithmetic(*left, *right, [](auto left, auto right) { return left + right; }, token);

            } else if (auto left_ptr = dynamic_cast<const String*>(left.get())) {
                auto right_ptr = dynamic_cast<const String*>(right.get());

                if (!right_ptr) {
                    throw RuntimeError(token, "Operands of string concatenation (+) must be of type String.");
                }

                temporary = String::New(left_ptr->value + right_ptr->value);

            } else {
                throw RuntimeError(token, "Operands of \"+\" must either be both of type Number (addition) or String (concatenation).");
            }

            break;
        }

        case TokenType::GREATER: {
            comparison(*left, *right, [](auto left, auto right) { return left > right;}, token);
            break;
        }

        case TokenType::GREATER_EQUAL: {
            comparison(*left, *right, [](auto left, auto right) { return left >= right;}, token);
            break;
        }

        case TokenType::LESS: {
            comparison(*left, *right, [](auto left, auto right) { return left < right;}, token);
            break;
        }

        case TokenType::LESS_EQUAL: {
            comparison(*left, *right, [](auto left, auto right) { return left <= right;}, token);
            break;
        }

        case TokenType::BANG_EQUAL: {
            temporary = Boolean::New(*left != *right);
            break;
        }

        case TokenType::EQUAL_EQUAL: {
            temporary = Boolean::New(*left == *right);
            break;
        }

        default: ; // Unreachable
    }
}

void Interpreter::arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token) {
    auto left_ptr = dynamic_cast<const Number*>(&left);
    auto right_ptr = dynamic_cast<const Number*>(&right);

    if (!(left_ptr && right_ptr)) {
        throw RuntimeError(token, "Operands of arithmetic operation (+, -, *, /) must be of type Number.");
    }

    auto result = op(left_ptr->value, right_ptr->value);
    temporary = Number::New(result);
}


void Interpreter::comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token) {
    auto left_ptr = dynamic_cast<const Number*>(&left);
    auto right_ptr = dynamic_cast<const Number*>(&right);

    if (!(left_ptr && right_ptr)) {
        throw RuntimeError(token, "Operands of arithmetic comparison (>, >=, <, <=) must be of type Number.");
    }

    auto result = op(left_ptr->value, right_ptr->value);
    temporary = Boolean::New(result);
}

const Number* asNumber(const LoxObject &object) {
    if (auto number = dynamic_cast<const Number*>(&object)) {
        return number;
    } else {
        return nullptr;
    }
}

void Interpreter::visit(const Literal &expression) {
    const string &lexeme = expression.token->lexeme;

    switch (expression.token->type) {

        case TokenType::NIL: {
            temporary = Nil::New();
            break;
        }

        case TokenType::NUMBER: {
            double value = stod(lexeme);

            temporary = Number::New(value);
            break;
        }

        case TokenType::STRING: {
            string value = lexeme.substr(1, lexeme.length() - 2);

            temporary = String::New(value);
            break;
        }

        case TokenType::TRUE: {
            temporary = Boolean::New(true);
            break;
        }

        case TokenType::FALSE: {
            temporary = Boolean::New(false);
            break;
        }

        default: ;
    }
}

void Interpreter::visit(const Logical &expression) {
    evaluate(*expression.left);

    if (temporary->isTruthy()) {
        if (expression.token->type == TokenType::AND) {
            // a and b = b if a, else a
            evaluate(*expression.right);
        }
    } else {
        if (expression.token->type == TokenType::OR) {
            // a or b = a if a, else b
            evaluate(*expression.right);
        }
    }
}

void Interpreter::visit(const Grouping &expression) {
    evaluate(*expression.content);
}

void Interpreter::visit(const Variable &expression) {
    // We have to copy here, other wise expressions like (-a) would change the state of a
    auto value = environment->get(*expression.name);
    temporary = move(value->clone());
}

void Interpreter::visit(const Assign &expression) {
    evaluate(*expression.value);

    environment->assign(*expression.name, temporary);
}

inline void Interpreter::evaluate(const Expression &expression) {
    expression.accept(*this);
}
