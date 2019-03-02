//
// Created by Lucas Wolf on 2019-02-26.
//

#include "Environment.h"

#include "interpreter/RuntimeError.h"

using namespace std;

// Constructors

Environment::Environment()
    : enclosing{nullptr} {}

Environment::Environment(Environment_ptr enclosing)
    : enclosing{move(enclosing)} {}


// Factory Functions

Environment_ptr Environment::New() {
    return make_shared<Environment>();
}

Environment_ptr Environment::New(Environment_ptr enclosing) {
    return make_shared<Environment>(move(enclosing));
}


void Environment::define(const string &name, shared_ptr<LoxObject> value) {
    values[name] = move(value);
}

shared_ptr<LoxObject> Environment::get(const Token &name) {
    // try to retrieve the value referred to by name
    auto value = values.find(name.lexeme);

    // if a value exists, return it
    if (value != values.end()) {
        return value->second;
    }

    // if no value has been found yet, recursively look it up in the enclosing scope
    if (enclosing) {
        return enclosing->get(name);
    }

    // if it still has not been found, the variable doesn't exist
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