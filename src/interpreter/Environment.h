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

// A lexical space in which variables live
class Environment {

public:
    std::unique_ptr<Environment> enclosing;

    void define(const std::string &name, std::shared_ptr<LoxObject> value);
    void assign(const Token &name, std::shared_ptr<LoxObject> value);
    std::shared_ptr<LoxObject> get(const Token &name);

    Environment();
    Environment(std::unique_ptr<Environment> enclosing);

private:
    std::unordered_map<std::string, std::shared_ptr<LoxObject>> values;
};


#endif //LOX_INTERPRETER_ENVIRONMENT_H
