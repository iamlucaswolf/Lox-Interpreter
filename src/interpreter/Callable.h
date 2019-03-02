//
// Created by Lucas Wolf on 2019-03-01.
//

#ifndef LOX_INTERPRETER_CALLABLE_H
#define LOX_INTERPRETER_CALLABLE_H

#include "LoxObject.h"
#include "Interpreter.h"

#include <functional>
#include <memory>

using NativeCallback = std::function<Object_ptr(Interpreter&, std::vector<Object_ptr>&)>;

// Anything that can be called in Lox
class Callable : public LoxObject {

public:
    virtual Object_ptr call(Interpreter &interpreter, std::vector<Object_ptr> &arguments) const = 0;
    virtual int arity() const = 0;
};


// Functions directly implemented by the Interpreter
class Native : public Callable {

private:
    const int _arity;
    NativeCallback callback;

    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    explicit Native(int arity, NativeCallback callback);
    static std::shared_ptr<Native> New(int arity, NativeCallback callback);

    Object_ptr clone() const override;

    int arity() const override;
    Object_ptr call(Interpreter &interpreter, std::vector<Object_ptr> &arguments) const override;
};

// Functions declared by Users
class FunctionObject : public Callable {

private:
    std::shared_ptr<Function> declaration;
    Environment_ptr closure;

    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    explicit FunctionObject(std::shared_ptr<Function> declaration, Environment_ptr closure);
    static std::shared_ptr<FunctionObject> New(std::shared_ptr<Function> declaration, Environment_ptr closure);

    Object_ptr clone() const override;

    int arity() const override;
    Object_ptr call(Interpreter &interpreter, std::vector<Object_ptr> &arguments) const override;
};


#endif //LOX_INTERPRETER_CALLABLE_H
