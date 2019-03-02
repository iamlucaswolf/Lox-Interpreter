//
// Created by Lucas Wolf on 2019-02-22.
//

#ifndef LOX_INTERPRETER_VALUE_H
#define LOX_INTERPRETER_VALUE_H

#include <string>
#include <iostream>

struct LoxObject;
using Object_ptr = std::shared_ptr<LoxObject>;


struct LoxObject {

private:
    virtual std::ostream& print(std::ostream &out) const = 0;
    virtual bool equals(const LoxObject &object) const = 0;

public:
    virtual ~LoxObject() = default;
    virtual Object_ptr clone() const = 0;

    virtual bool isTruthy() const;

    friend std::ostream& operator<< (std::ostream &out, const LoxObject &object);
    friend bool operator== (const LoxObject &left, const LoxObject &right);
    friend bool operator!= (const LoxObject &left, const LoxObject &right);
};


struct Number : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    double value;

    explicit Number(double value) : value{value} {};
    static std::shared_ptr<Number> New(double value);

    Object_ptr clone() const override;
};


struct Boolean : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    bool value;

    explicit Boolean(bool value) : value{value} {};
    static std::shared_ptr<Boolean> New(bool value);

    bool isTruthy() const override;
    Object_ptr clone() const override;
};


struct String : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    std::string value;

    explicit String(std::string value) : value{ std::move(value) } {};
    static std::shared_ptr<String> New(std::string value);

    Object_ptr clone() const override;
};


struct Nil : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    static std::shared_ptr<Nil> New();

    bool isTruthy() const override;
    Object_ptr clone() const override;
};


#endif //LOX_INTERPRETER_VALUE_H
