//
// Created by Lucas Wolf on 2019-02-14.
//

#include "parser.h"

#include "data/statement.h"

#include <sstream>

using namespace std;


// DISCLAIMER: The code below still has a lot of TODOs and rough edges. I'll leave them in for now, as some pieces which
// seem redundant now might prove valuable later. Let's see...

// Helper class
class Parser {

public:

    explicit Parser(vector<Token_ptr> &&tokens);
    vector<Statement_ptr> parse();

private:

    // The list of Tokens to be parsed into an AST
    vector<Token_ptr> tokens;

    vector<Token_ptr>::iterator current;

    // A log of ParseErrors that occured during parsing
    vector<ParseError> errors;

    // Statement productions
    Statement_ptr declaration();
    Statement_ptr varDeclaration();
    Statement_ptr statement();
    Statement_ptr print();
    Statement_ptr block();
    Statement_ptr expressionStatement();

    // Expression productions
    Expression_ptr expression();
    Expression_ptr assignment();
    Expression_ptr equality();
    Expression_ptr comparison();
    Expression_ptr addition();
    Expression_ptr multiplication();
    Expression_ptr unary();
    Expression_ptr primary();

    // Production patterns
    Expression_ptr binary(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types);

    // Panic mode recovery
    void synchronize();

    // Helper functions
    // TODO refactor, especially with regards to which functions pass ownership!

    bool match(initializer_list<TokenType> types);
    bool check(const TokenType &type);
    bool isAtEnd();
    const Token& advance();
    Token_ptr advance_own();
    const Token& peek();
    const Token& previous();
    Token_ptr previous_own();
    const Token& expect(const TokenType &type, const string &message);
    Token_ptr expect_own(const TokenType &type, const string &message);
};

vector<Statement_ptr> parse(vector<Token_ptr> &&tokens) {
    Parser parser { move(tokens) };
    return parser.parse();
}

Parser::Parser(vector<Token_ptr> &&tokens) : tokens{move(tokens)} {
    // We can't do this in the initializer list, because we need to make sure that the Tokens are moved first
    current = this->tokens.begin();
}

vector<Statement_ptr> Parser::parse() {
    vector<Statement_ptr> statements;

    while (!isAtEnd()) {
        Statement_ptr statement = declaration();

        if (statement) {
            statements.push_back(move(statement));
        }
    }

    if (!errors.empty()) {
        throw ParserError(move(errors));
    }

    return statements;
}


Statement_ptr Parser::declaration() {
    try {
        if (match({TokenType::VAR})) {
            return varDeclaration();
        }

        return statement();
    } catch (const ParseError &e) {
        // Log error
        errors.push_back(e);

        // enter panic mode
        synchronize();

        return nullptr;
    }
}


Statement_ptr Parser::varDeclaration() {
    Token_ptr name = expect_own(TokenType::IDENTIFIER, "Expect variable name.");

    Expression_ptr initializer = match({TokenType::EQUAL}) ? expression() : nullptr;

    expect(TokenType::SEMICOLON, "Expect \';\' after variable declaration.");
    return make_unique<Var>(move(name), move(initializer));
}


Statement_ptr Parser::statement() {
    if (match({TokenType::PRINT})) {
        return print();
    }

    if (match({TokenType::LEFT_BRACE})) {
        return block();
    }

    return expressionStatement();
}

Statement_ptr Parser::expressionStatement() {
    auto value = expression();

    expect(TokenType::SEMICOLON, "Expect \';\' after expression.");
    return make_unique<ExpressionStatement>(move(value));
}

Statement_ptr Parser::print() {
    auto value = expression();

    expect(TokenType::SEMICOLON, "Expect \';\' after value.");
    return make_unique<Print>(move(value));
}

Statement_ptr Parser::block() {
    vector<Statement_ptr> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    expect(TokenType::RIGHT_BRACE, "Expect \'}\' after block.");
    return make_unique<Block>(move(statements));
}

Expression_ptr Parser::expression() {
    return assignment();
}


Expression_ptr Parser::assignment() {

    // parse higher-precedence expression
    auto expression = equality();

    // if we encounter an equals sign
    if (match({TokenType::EQUAL})) {

        // get the equals Token
        auto equals = previous();

        // parse nested assignment (assignments are right-associative)
        auto value = assignment();

        // if the left-hand expression was an l-value (right now, a variable)
        if (auto variable = dynamic_cast<Variable*>(expression.get())) {
            return make_unique<Assign>(move(variable->name), move(value));
        }

        // since assignments are expressions, a semicolon is expected immediately afterwards anyway, so no need to panic
        throw ParseError(equals, "Invalid assignment target.");
    }

    return expression;
}


Expression_ptr Parser::equality() {
    return binary(
        bind(&Parser::comparison, this),
        { TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL }
    );
}

Expression_ptr Parser::comparison() {
    return binary(
        bind(&Parser::addition, this),
        { TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL }
    );
}

Expression_ptr Parser::addition() {
    return binary(
        bind(&Parser::multiplication, this),
        { TokenType::MINUS, TokenType::PLUS }
    );
}

Expression_ptr Parser::multiplication() {
    return binary(
        bind(&Parser::unary, this),
        { TokenType::SLASH, TokenType::STAR }
    );
}

Expression_ptr Parser::binary(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types) {
    auto left = sub_production();

    while (match(operator_types)) {

        auto operator_ = previous_own();
        auto right = sub_production();

        left = make_unique<Binary>(move(left), move(operator_), move(right));
    }

    return left;
}

Expression_ptr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto prefix = previous_own();
        auto right = unary();
        return make_unique<Unary>(move(prefix), move(right));
    }

    return primary();
}

Expression_ptr Parser::primary() {

    auto types = {
            TokenType::FALSE,
            TokenType::TRUE,
            TokenType::NIL,
            TokenType::NUMBER,
            TokenType::STRING
    };

    if (match(types)) {
        return make_unique<Literal>(move(previous_own()));
    }

    if (match({TokenType::IDENTIFIER})) {
        return make_unique<Variable>(move(previous_own()));
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expression = this->expression();

        expect(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return make_unique<Grouping>(move(expression));
    }

    throw ParseError(peek(), "Expect expression.");
}


void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) {
            return;
        }

        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                advance();
        }
    }
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

bool Parser::check(const TokenType &type) {
    if (isAtEnd()) {
        return false;
    }

    return peek().type == type;
}

// TODO rather check if iterator is at end, then we don't need EOF anymore
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }

    return previous();
}

Token_ptr Parser::advance_own() {
    if (!isAtEnd()) {
        ++current;
    }

    return previous_own();
}

const Token& Parser::peek() {
    return **current;
}

const Token& Parser::previous() {
    //return tokens.at(current - 1);
    return **(current - 1);
}

Token_ptr Parser::previous_own() {
    return move(*(current - 1));
}

const Token& Parser::expect(const TokenType &type, const string &message) {
    if (check(type)) {
        return advance();
    }

    // Enter panic mode
    throw ParseError{peek(), message};
}

Token_ptr Parser::expect_own(const TokenType &type, const string &message) {
    if (check(type)) {
        return advance_own();
    }

    // Enter panic mode
    throw ParseError{peek(), message};
}