//
// Created by Lucas Wolf on 2019-02-26.
//

#include "Environment.h"

#include "interpreter/RuntimeError.h"

using namespace std;

Environment::Environment()
    : enclosing{nullptr} {}

Environment::Environment(std::unique_ptr<Environment> enclosing)
    : enclosing{move(enclosing)} {}

void Environment::define(const string &name, shared_ptr<LoxObject> value) {
    values[name] = move(value);
}

shared_ptr<LoxObject> Environment::get(const Token &name) {
    auto value = values.find(name.lexeme);

    if (value != values.end()) {
        return value->second;
    }

    if (enclosing) {
        return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable \'" + name.lexeme + "\'.");
}

void Environment::assign(const Token &name, shared_ptr<LoxObject> value) {

    if (values.find(name.lexeme) != values.end()) {
        values[name.lexeme] = move(value);
        return;
    }

    if (enclosing) {
        enclosing->assign(name, move(value));
        return;
    }

    throw RuntimeError(name, "Undefined variable \'" + name.lexeme + "\'.");
}