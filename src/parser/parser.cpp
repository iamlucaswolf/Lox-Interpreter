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
    // TODO do we really need to own the entire vector? Isn't the iterator sufficient?
    vector<Token_ptr> tokens;
    vector<Token_ptr>::iterator current;

    // A log of ParseErrors that occured during parsing
    vector<ParseError> errors;

    // Statement productions
    Statement_ptr declaration();
    Statement_ptr funDeclaration(const std::string &kind);
    Statement_ptr varDeclaration();
    Statement_ptr statement();
    Statement_ptr print();
    Statement_ptr block();
    Statement_ptr expressionStatement();
    Statement_ptr ifStatement();
    Statement_ptr whileStatement();
    Statement_ptr forStatement();
    Statement_ptr returnStatement();

    // Expression productions
    Expression_ptr expression();
    Expression_ptr assignment();
    Expression_ptr equality();
    Expression_ptr comparison();
    Expression_ptr addition();
    Expression_ptr multiplication();
    Expression_ptr unary();
    Expression_ptr primary();
    Expression_ptr _or();
    Expression_ptr _and();
    Expression_ptr call();

    // Helper productions
    Expression_ptr finishCall(Expression_ptr callee);
    Expression_ptr binary(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types);
    Expression_ptr logical(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types);

    // Panic mode recovery
    void synchronize();

    // Helper functions
    // TODO refactor, especially with regards to which functions pass ownership!

    bool match(initializer_list<TokenType> types);
    bool check(const TokenType &type);
    bool isAtEnd();
//    const Token& advance();
    Token_ptr advance();
    const Token& peek();
//    const Token& previous();
    Token_ptr previous();
//    const Token& expect(const TokenType &type, const string &message);
    Token_ptr expect(const TokenType &type, const string &message);
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
        if (match({TokenType::FUN})) {
            return funDeclaration("function");
        }

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

Statement_ptr Parser::funDeclaration(const std::string &kind) {
    auto name = expect(TokenType::IDENTIFIER, "Expect " + kind + " name.");

    expect(TokenType::LEFT_PAREN, "Expect \'(\' after " + kind + " name.");

    vector<Token_ptr> parameters;

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 8) {
                throw ParseError(peek(), "Cannot have more than 8 paramters.");
            }

            auto parameter = expect(TokenType::IDENTIFIER, "Expect paramter name.");
            parameters.push_back(move(parameter));
        } while (match({TokenType::COMMA}));
    }

    expect(TokenType::RIGHT_PAREN, "Expect \')\' after parameters.");
    expect(TokenType::LEFT_BRACE, "Expect \'{\' before " + kind + " body.");

    // TODO it would be better if Function took a Statement_ptr, then we could pass the block directly
    // TODO for now it's okay because `body` is tied to local scope
    auto body = block();
    auto body_ptr = dynamic_cast<Block*>(body.get());

    return Function::New(move(name), move(parameters), move(body_ptr->statements));
}


Statement_ptr Parser::varDeclaration() {
    Token_ptr name = expect(TokenType::IDENTIFIER, "Expect variable name.");

    Expression_ptr initializer = match({TokenType::EQUAL}) ? expression() : nullptr;

    expect(TokenType::SEMICOLON, "Expect \';\' after variable declaration.");
    return Var::New(move(name), move(initializer));
}


Statement_ptr Parser::statement() {
    if (match({TokenType::IF})) {
        return ifStatement();
    }

    if (match({TokenType::WHILE})) {
        return whileStatement();
    }

    if (match({TokenType::FOR})) {
        return forStatement();
    }

    if (match({TokenType::PRINT})) {
        return print();
    }

    if (match({TokenType::RETURN})) {
        return returnStatement();
    }

    if (match({TokenType::LEFT_BRACE})) {
        return block();
    }

    return expressionStatement();
}


Statement_ptr Parser::ifStatement() {
    expect(TokenType::LEFT_PAREN, R"(Expect '(' after 'if'.)");
    Expression_ptr condition = expression();
    expect(TokenType::RIGHT_PAREN, "Expect \')\' after if condition.");

    Statement_ptr thenBranch = statement();
    Statement_ptr elseBranch = nullptr;

    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return If::New(move(condition), move(thenBranch), move(elseBranch));
}


Statement_ptr Parser::expressionStatement() {
    auto value = expression();

    expect(TokenType::SEMICOLON, "Expect \';\' after expression.");
    return ExpressionStatement::New(move(value));
}


Statement_ptr Parser::print() {
    auto value = expression();

    expect(TokenType::SEMICOLON, "Expect \';\' after value.");
    return Print::New(move(value));
}


Statement_ptr Parser::block() {
    vector<Statement_ptr> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    expect(TokenType::RIGHT_BRACE, "Expect \'}\' after block.");

    return Block::New(move(statements));
}


Statement_ptr Parser::whileStatement() {
    expect(TokenType::LEFT_PAREN, R"(Expect '(' after 'while'.)");
    Expression_ptr condition = expression();
    expect(TokenType::RIGHT_PAREN, "Expect \')\' after if condition.");

    Statement_ptr body = statement();

    return While::New(move(condition), move(body));
}

Statement_ptr Parser::forStatement() {
    expect(TokenType::LEFT_PAREN, R"(Expect '(' after 'while'.)");

    // Loop initializer
    auto initializer = match({TokenType::SEMICOLON})
        ? nullptr
        : match({TokenType::VAR})
            ? varDeclaration()
            : expressionStatement();

    // Loop condition
    auto condition = check(TokenType::SEMICOLON)
        ? Literal::New(Token::New(TokenType::TRUE, "true", previous()->line))
        : expression();


    expect(TokenType::SEMICOLON, "Expect \';\' after loop condition.");

    // Increment
    auto increment = check(TokenType::RIGHT_PAREN) ? nullptr : expression();
    expect(TokenType::RIGHT_PAREN, "Expect \')\' after for clauses.");

    // Body
    auto body = statement();

    // Piece everything together
    if (increment) {
        // have to push_back every element indiviudally, since initializer_lists don't work with move-only types
        vector<Statement_ptr> statements;
        statements.push_back(move(body));
        statements.push_back(ExpressionStatement::New(move(increment)));

        body = Block::New(move(statements));
    }

    body = While::New(move(condition), move(body));

    if (initializer) {
        vector<Statement_ptr> statements;
        statements.push_back(move(initializer));
        statements.push_back(move(body));

        body = Block::New(move(statements));
    }

    return body;
}


Statement_ptr Parser::returnStatement() {
    Token_ptr keyword = previous();
    Expression_ptr value = nullptr;

    if (not check(TokenType::SEMICOLON)) {
        value = expression();
    }

    expect(TokenType::SEMICOLON, "Expect \';\' after return value.");
    return Return::New(move(keyword), move(value));
}


Expression_ptr Parser::expression() {
    return assignment();
}


Expression_ptr Parser::assignment() {

    // parse higher-precedence expression
    auto expression = _or();

    // if we encounter an equals sign
    if (match({TokenType::EQUAL})) {

        // get the equals Token
        auto equals = previous();

        // parse nested assignment (assignments are right-associative)
        auto value = assignment();

        // if the left-hand expression was an l-value (right now, a variable)
        if (auto variable = dynamic_cast<Variable*>(expression.get())) {
            return Assign::New(move(variable->name), move(value));
        }

        // since assignments are expressions, a semicolon is expected immediately afterwards anyway, so no need to panic
        throw ParseError(*equals, "Invalid assignment target.");
    }

    return expression;
}


Expression_ptr Parser::_or() {
    return logical(
        bind(&Parser::_and, this),
        { TokenType::OR }
    );
}

Expression_ptr Parser::_and() {
    return logical(
        bind(&Parser::equality, this),
        { TokenType::AND }
    );
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

        auto operator_ = previous();
        auto right = sub_production();

        left = Binary::New(move(left), move(operator_), move(right));
    }

    return left;
}

Expression_ptr Parser::logical(const function<Expression_ptr()> &sub_production, initializer_list<TokenType> operator_types) {
    auto left = sub_production();

    while (match(operator_types)) {

        auto operator_ = previous();
        auto right = sub_production();

        left = Logical::New(move(left), move(operator_), move(right));
    }

    return left;
}

Expression_ptr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto prefix = previous();
        auto right = unary();
        return Unary::New(move(prefix), move(right));
    }

    return call();
}

Expression_ptr Parser::call() {
    auto expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(move(expr));
        } else {
            break;
        }
    }

    return expr;
}


Expression_ptr Parser::finishCall(Expression_ptr callee) {
    vector<Expression_ptr> arguments;

    if (not check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 8) {
                errors.emplace_back(ParseError{peek(), "Cannot have more than 8 arguments."});
            }

            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    auto paren = expect(TokenType::RIGHT_PAREN, "Expect \')\' after arguments.");

    return Call::New(move(callee), move(paren), move(arguments));
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
        return Literal::New(move(previous()));
    }

    if (match({TokenType::IDENTIFIER})) {
        return Variable::New(move(previous()));
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expression = this->expression();

        expect(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return Grouping::New(move(expression));
    }

    throw ParseError(peek(), "Expect expression.");
}




void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous()->type == TokenType::SEMICOLON) {
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

//const Token& Parser::advance() {
//    if (!isAtEnd()) {
//        current++;
//    }
//
//    return previous();
//}

Token_ptr Parser::advance() {
    if (!isAtEnd()) {
        ++current;
    }

    return previous();
}

const Token& Parser::peek() {
    return **current;
}

//const Token& Parser::previous() {
//    //return tokens.at(current - 1);
//    return **(current - 1);
//}

Token_ptr Parser::previous() {
    return *(current - 1);
}

//const Token& Parser::expect(const TokenType &type, const string &message) {
//    if (check(type)) {
//        return advance();
//    }
//
//    // Enter panic mode
//    throw ParseError{peek(), message};
//}

Token_ptr Parser::expect(const TokenType &type, const string &message) {
    if (check(type)) {
        return advance();
    }

    // Enter panic mode
    throw ParseError{peek(), message};
}