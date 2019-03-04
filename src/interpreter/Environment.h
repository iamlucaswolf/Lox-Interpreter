//
// Created by Lucas Wolf on 2019-02-26.
//

#ifndef LOX_INTERPRETER_ENVIRONMENT_H
#define LOX_INTERPRETER_ENVIRONMENT_H

#include "interpreter/LoxObject.h"
#include "data/token.h"

#include <string>
#include <unordered_map>
#include <memory>

// Forward declarations
class Environment;

// Pointer declarations
using Environment_ptr = std::shared_ptr<Environment>;


// Lexical space in which variables live
class Environment {

public:
    Environment();
    Environment(Environment_ptr enclosing);
    static Environment_ptr New();
    static Environment_ptr New(Environment_ptr enclosing);


    // associates a new variable with a name
    void define(const std::string &name, Object_ptr value);

    // assigns a new value to an existing variable
    void assign(const Token &name, Object_ptr value);
    void assignAt(int distance, const Token &name, Object_ptr value);

    // retrieve the value associated with a name
    Object_ptr get(const Token &name);
    Object_ptr getAt(int distance, std::string name);

    // the environment in which this environment is nested in
    Environment_ptr enclosing;

private:
    std::unordered_map<std::string, Object_ptr> values;
    Environment& ancestor(int distance);
};





#endif //LOX_INTERPRETER_ENVIRONMENT_H
