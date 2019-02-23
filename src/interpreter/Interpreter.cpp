//
// Created by Lucas Wolf on 2019-02-22.
//

#include "Interpreter.h"

using namespace std;

void Interpreter::interpret(const Expression &expression) {
    result.reset();

    evaluate(expression);
    cout << *result << "\n";
}

void Interpreter::visit(const Unary &expression) {
    evaluate(*expression.operand);

    const Token &token = *expression.token;

    switch (token.type) {

        // Unary minus (-)
        case TokenType::MINUS: {
            auto number = dynamic_cast<Number*>(result.get());

            if (!number) {
                throw RuntimeError(token, "Operand of unary minus (-) must be of type Number");
            }

            number->value = -(number->value);
            break;
        }

        // Logical negation (!)
        case TokenType::BANG: {
            result = make_unique<Boolean>(result->isTruthy());
            break;
        }

        default: ; // Unreachable
    }
}

void Interpreter::visit(const Binary &expression) {

    // Evaluate left and right-hand operands
    evaluate(*expression.left);
    auto left = move(result);

    evaluate(*expression.right);
    auto right = move(result);

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

                result = make_unique<String>(left_ptr->value + right_ptr->value);

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
            result = make_unique<Boolean>(*left != *right);
            break;
        }

        case TokenType::EQUAL_EQUAL: {
            result = make_unique<Boolean>(*left == *right);
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
    this->result = make_unique<Number>(result);
}


void Interpreter::comparison(const LoxObject &left, const LoxObject &right, const Comparison &op, const Token &token) {
    auto left_ptr = dynamic_cast<const Number*>(&left);
    auto right_ptr = dynamic_cast<const Number*>(&right);

    if (!(left_ptr && right_ptr)) {
        throw RuntimeError(token, "Operands of arithmetic comparison (>, >=, <, <=) must be of type Number");
    }

    auto result = op(left_ptr->value, right_ptr->value);
    this->result = make_unique<Boolean>(result);
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

            result = make_unique<Number>(value);
            break;
        }

        case TokenType::STRING: {
            string value = lexeme.substr(1, lexeme.length() - 2);

            result = make_unique<String>(value);
            break;
        }

        case TokenType::TRUE: {
            result = make_unique<Boolean>(true);
            break;
        }

        case TokenType::FALSE: {
            result = make_unique<Boolean>(false);
            break;
        }

        default: ;
    }
}

void Interpreter::visit(const Grouping &expression) {
    evaluate(*expression.content);
}

inline void Interpreter::evaluate(const Expression &expression) {
    expression.accept(*this);
}

// TODO move to own .cpp file
RuntimeError::RuntimeError(const Token &token, std::string message) : token{token}, message{ std::move(message) } {}

std::string RuntimeError::what() const noexcept {
    return report (token.line, "", message);
}