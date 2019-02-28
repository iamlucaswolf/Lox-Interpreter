//
// Created by Lucas Wolf on 2019-02-22.
//

#include "Interpreter.h"
#include "RuntimeError.h"

using namespace std;

void Interpreter::interpret(const vector<unique_ptr<Statement>> &statements) {
    
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
    // WARNING originally in executeBlock() see chapter 8

    try {
        // Create a new environment enclosed by the current one
        environment = make_unique<Environment>(move(environment));

        for (const auto &s : statement.statements) {
            execute(*s);
        }
    } catch (const LoxError &e) {
        // Restore the old environment
        environment = move(environment->enclosing);
        throw;
    }

    // TODO better finally-like solution?
    // Restore the old environment
    environment = move(environment->enclosing);
}

void Interpreter::visit(const Print &statement) {
    evaluate(*statement.expression);
    cout << *temporary << "\n";
}

void Interpreter::visit(const Var &statement) {

    if (statement.initializer) {
        evaluate(*statement.initializer);
    } else {
        temporary = make_shared<Nil>();
    }

    environment->define(statement.name->lexeme, temporary);
}

void Interpreter::visit(const Unary &expression) {
    evaluate(*expression.operand);

    const Token &token = *expression.token;

    switch (token.type) {

        // Unary minus (-)
        case TokenType::MINUS: {
            auto number = dynamic_cast<Number*>(temporary.get());

            if (!number) {
                throw RuntimeError(token, "Operand of unary minus (-) must be of type Number");
            }

            number->value = -(number->value);
            break;
        }

        // Logical negation (!)
        case TokenType::BANG: {
            temporary = make_shared<Boolean>(temporary->isTruthy());
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
                    throw RuntimeError(token, "Operands of string concatenation (+) must be of type String");
                }

                temporary = make_shared<String>(left_ptr->value + right_ptr->value);

            } else {
                throw RuntimeError(token, "Operands of \"+\" must either be both of type Number (addition) or String (concatenation");
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
            temporary = make_shared<Boolean>(*left != *right);
            break;
        }

        case TokenType::EQUAL_EQUAL: {
            temporary = make_unique<Boolean>(*left == *right);
            break;
        }

        default: ; // Unreachable
    }
}

void Interpreter::arithmetic(const LoxObject &left, const LoxObject &right, const Arithmetic &op, const Token &token) {
    auto left_ptr = dynamic_cast<const Number*>(&left);
    auto right_ptr = dynamic_cast<const Number*>(&right);

    if (!(left_ptr && right_ptr)) {
        throw RuntimeError(token, "Operands of arithmetic operation (+, -, *, /) must be of type Number");
    }

    auto result = op(left_ptr->value, right_ptr->value);
    temporary = make_shared<Number>(result);
}


void Interpreter::comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token) {
    auto left_ptr = dynamic_cast<const Number*>(&left);
    auto right_ptr = dynamic_cast<const Number*>(&right);

    if (!(left_ptr && right_ptr)) {
        throw RuntimeError(token, "Operands of arithmetic comparison (>, >=, <, <=) must be of type Number");
    }

    auto result = op(left_ptr->value, right_ptr->value);
    temporary = make_shared<Boolean>(result);
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

        case TokenType::NUMBER: {
            double value = stod(lexeme);

            temporary = make_shared<Number>(value);
            break;
        }

        case TokenType::STRING: {
            string value = lexeme.substr(1, lexeme.length() - 2);

            temporary = make_shared<String>(value);
            break;
        }

        case TokenType::TRUE: {
            temporary = make_shared<Boolean>(true);
            break;
        }

        case TokenType::FALSE: {
            temporary = make_shared<Boolean>(false);
            break;
        }

        default: ;
    }
}

void Interpreter::visit(const Grouping &expression) {
    evaluate(*expression.content);
}

void Interpreter::visit(const Variable &expression) {
    // We have to copy here, other wise expressions like (-a) would change the state of a
    auto value = environment->get(*expression.name);
    temporary = value->clone();
}

void Interpreter::visit(const Assign &expression) {
    evaluate(*expression.value);

    environment->assign(*expression.name, temporary);
}

inline void Interpreter::evaluate(const Expression &expression) {
    expression.accept(*this);
}
