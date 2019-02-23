//
// Created by Lucas Wolf on 2019-02-22.
//

#ifndef LOX_INTERPRETER_VALUE_H
#define LOX_INTERPRETER_VALUE_H

#include <string>
#include <iostream>

struct LoxObject {

private:
    virtual std::ostream& print(std::ostream &out) const = 0;
    virtual bool equals(const LoxObject &object) const = 0;

public:
    virtual ~LoxObject() = default;

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
};


struct Boolean : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    bool value;

    explicit Boolean(bool value) : value{value} {};
    bool isTruthy() const override;
};


struct String : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    std::string value;

    explicit String(std::string value) : value{ std::move(value) } {};
};


struct Nil : public LoxObject {

private:
    std::ostream& print(std::ostream &out) const override;
    bool equals(const LoxObject &object) const override;

public:
    bool isTruthy() const override;
};

#endif //LOX_INTERPRETER_VALUE_H
