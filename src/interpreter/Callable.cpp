//
// Created by Lucas Wolf on 2019-03-01.
//

#include "Callable.h"

using namespace std;

// Deep copy

shared_ptr<LoxObject> Native::clone() const {
    return make_shared<Native>(*this);
}

Object_ptr FunctionObject::clone() const {
    return make_shared<FunctionObject>(*this);
}

// Printing

ostream& Native::print(ostream &out) const {
    out << "<native function>";
    return out;
}

ostream& FunctionObject::print(ostream &out) const {
    out << "<fn " + declaration->name->lexeme + ">";
    return out;
}

// Equality

bool Native::equals(const LoxObject &object) const {
    // We can't compare Natives since std::function doesn't allow for equality checking either
    return false;
}

bool FunctionObject::equals(const LoxObject &object) const {
    if (auto other = dynamic_cast<const FunctionObject*>(&object)) {
        return this->declaration == other->declaration;
    }

    return false;
}

// Construction

Native::Native(int arity, NativeCallback callback)
    : _arity{arity}, callback{move(callback)} {}

FunctionObject::FunctionObject(shared_ptr<Function> declaration, Environment_ptr closure)
    : declaration{move(declaration)}, closure{move(closure)} {}


// Factory functions

shared_ptr<Native> Native::New(int arity, NativeCallback callback) {
    return make_shared<Native>(arity, move(callback));
}

shared_ptr<FunctionObject> FunctionObject::New(shared_ptr<Function> declaration, Environment_ptr closure) {
    return make_shared<FunctionObject>(move(declaration), move(closure));
}

// Arity

int Native::arity() const {
    return _arity;
}

int FunctionObject::arity() const {
    return declaration->parameters.size();
}


// Invocation

Object_ptr Native::call(Interpreter &interpreter, vector<Object_ptr> &arguments) const {
    return callback(interpreter, arguments);
}

Object_ptr FunctionObject::call(Interpreter &interpreter, std::vector<Object_ptr> &arguments) const {
    // can't move here, because we want interpreter to retain its globals
    auto environment = Environment::New(closure);

    for (int i = 0; i < declaration->parameters.size(); ++i) {
        auto argument = arguments.at(i);
        environment->define(declaration->parameters.at(i)->lexeme, move(argument));
    }

    try {
        interpreter.executeBlock(declaration->body, move(environment));
    } catch (const ReturnValue &returnValue) {
        return move(returnValue.value);
    }

    return Nil::New();
}


