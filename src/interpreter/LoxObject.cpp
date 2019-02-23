//
// Created by Lucas Wolf on 2019-02-23.
//

#include "LoxObject.h"

#include <iostream>

using namespace std;

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

