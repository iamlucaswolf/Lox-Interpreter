//
// Created by Lucas Wolf on 2019-02-23.
//

#include "LoxObject.h"

#include <iostream>

using namespace std;

// Factory Functions

shared_ptr<Number> Number::New(double value) {
    return make_shared<Number>(value);   
}

shared_ptr<Boolean> Boolean::New(bool value) {
    return make_shared<Boolean>(value);
}

shared_ptr<String> String::New(string value) {
    return make_shared<String>(move(value));
}

shared_ptr<Nil> Nil::New() {
    return make_shared<Nil>();
}


// Deep copy

Object_ptr Number::clone() const {
    return make_shared<Number>(*this);
}

Object_ptr Boolean::clone() const {
    return make_shared<Boolean>(*this);
}

Object_ptr String::clone() const {
    return make_shared<String>(*this);
}

Object_ptr Nil::clone() const {
    return make_shared<Nil>(*this);
}


// Truthiness

bool LoxObject::isTruthy() const {
    return true;
}

bool Boolean::isTruthy() const {
    return value;
}

bool Nil::isTruthy() const {
    return false;
}

// Printing to ostream

ostream& operator<<(ostream &out, const LoxObject &object) {
    return object.print(out);
}

ostream& Number::print(ostream &out) const {
    out << value;
    return out;
}

ostream& Boolean::print (ostream &out) const {
    out << boolalpha << value;
    return out;
}

ostream& String::print (ostream &out) const {
    out << value;
    return out;
}

ostream& Nil::print (ostream &out) const {
    out << "nil";
    return out;
}

// Equality

bool operator==(const LoxObject &left, const LoxObject &right) {
    return left.equals(right);
}

bool operator!=(const LoxObject &left, const LoxObject &right) {
    return !left.equals(right);
}

bool Number::equals(const LoxObject &object) const {
    if (auto other = dynamic_cast<const Number*>(&object)) {
        return this->value == other->value;
    }

    return false;
}

bool Boolean::equals(const LoxObject &object) const {
    if (auto other = dynamic_cast<const Boolean*>(&object)) {
        return this->value == other->value;
    }

    return false;
}

bool String::equals(const LoxObject &object) const {
    if (auto other = dynamic_cast<const String*>(&object)) {
        return this->value == other->value;
    }

    return false;
}

bool Nil::equals(const LoxObject &object) const {
    if (auto other = dynamic_cast<const Nil*>(&object)) {
        return true;
    }

    return false;
}