//
// Created by Lucas Wolf on 2019-02-14.
//

#include "parser.h"
#include <sstream>

using namespace std;


// DISCLAIMER: The code below still has a lot of TODOs and rough edges. I'll leave them in for now, as some pieces which
// seem redundant now might prove valuable later. Let's see...

// TODO move to respective datastructures?
using Token_ptr = shared_ptr<Token>;
using Expression_ptr = unique_ptr<Expression>;


// Helper class
class Parser {

    // The list of Tokens to be parsed into an AST
    vector<shared_ptr<Token>> &tokens;

    //int current = 0;
    vector<Token_ptr>::iterator current;

    // A log of ParseErrors that occured during parsing
    // TODO expand on best effort strategy
    vector<ParseError> errors;


    // Productions
    unique_ptr<Expression> expression();
    unique_ptr<Expression> equality();
    unique_ptr<Expression> comparison();
    unique_ptr<Expression> addition();
    unique_ptr<Expression> multiplication();
    unique_ptr<Expression> unary();
    unique_ptr<Expression> primary();

    Expression_ptr binary(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types);
    void synchronize();

    bool match(initializer_list<TokenType> types);
    bool check(TokenType type);
    bool isAtEnd();
    shared_ptr<Token> advance();
    shared_ptr<Token> peek();
    shared_ptr<Token> previous();
    shared_ptr<Token> expect(TokenType type, const string &message);


public:

    explicit Parser(vector<shared_ptr<Token>> &tokens) : tokens{tokens}, current {tokens.begin()} {}
    unique_ptr<Expression> parse();
};


unique_ptr<Expression> parse(vector<shared_ptr<Token>> &tokens) {
    Parser parser {tokens};
    return parser.parse();
}


unique_ptr<Expression> Parser::parse() {

    try {
        return expression();
    } catch (const ParseError &e) {
        errors.push_back(e);
    }

    throw ParserError { move(errors) };
}

unique_ptr<Expression> Parser::expression() {
    return equality();
}

unique_ptr<Expression> Parser::equality() {
    return binary(
        bind(&Parser::comparison, this),
        { TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL }
    );
}

unique_ptr<Expression> Parser::comparison() {
    return binary(
        bind(&Parser::addition, this),
        { TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL }
    );
}

unique_ptr<Expression> Parser::addition() {
    return binary(
        bind(&Parser::multiplication, this),
        { TokenType::MINUS, TokenType::PLUS }
    );
}

unique_ptr<Expression> Parser::multiplication() {
    return binary(
        bind(&Parser::unary, this),
        { TokenType::SLASH, TokenType::STAR }
    );
}

Expression_ptr Parser::binary(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types) {
    auto left = sub_production();

    while (match(operator_types)) {

        auto operator_ = previous();
        auto right = sub_production();

        left = make_unique<Binary>(move(left), operator_, move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto prefix = previous();
        auto right = unary();
        return make_unique<Unary>(prefix, move(right));
    }

    return primary();
}

unique_ptr<Expression> Parser::primary() {

    auto types = {
            TokenType::FALSE,
            TokenType::TRUE,
            TokenType::NIL,
            TokenType::NUMBER,
            TokenType::STRING
    };

    if (match(types)) {
        return make_unique<Literal>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expression = this->expression();

        expect(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return make_unique<Grouping>(move(expression));
    }

    throw ParseError(*peek(), "Expect expression.");
}


// TODO better idea: match() [report match on current position], consume() [return and advance] ?
bool Parser::match(initializer_list<TokenType> types) {

    for (const auto &type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }

    return peek()->type == type;
}

// TODO rather check if iterator is at end, then we don't need EOF anymore
bool Parser::isAtEnd() {
    return peek()->type == TokenType::END_OF_FILE;
}

shared_ptr<Token> Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }

    return previous();
}

// TODO isn't this redundant?
// TODO should we also have methods that return a normal (Token) pointer rather than an owning one?
shared_ptr<Token> Parser::peek() {
    return *current;
}

shared_ptr<Token> Parser::previous() {
    //return tokens.at(current - 1);
    return *(current - 1);
}

shared_ptr<Token> Parser::expect(TokenType type, const string &message) {
    if (check(type)) {
        return advance();
    }

    // Enter panic mode
    throw ParseError{*peek(), message};
}