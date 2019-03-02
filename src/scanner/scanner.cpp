//
// Created by Lucas Wolf on 2019-01-20.
//

#include "scanner.h"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>

using namespace std;

class Scanner {

public:
    explicit Scanner(const std::string& source);
    vector<Token_ptr> scan();

private:

    const std::string &source;
    vector<Token_ptr> tokens;

    string::const_iterator start;
    string::const_iterator current;

    int line {1};

    void nextToken();
    void scanString();
    void scanNumber();
    void scanIdentifier();

    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    bool isAtEnd();
    void addToken(const TokenType &type);
};

bool isDigit(char c);
bool isAlpha(char c);
bool isAlphaNumeric(char c);

const std::unordered_map<std::string, TokenType> keywords {
        { "and", TokenType::AND },
        { "class", TokenType::CLASS },
        { "else", TokenType::ELSE },
        { "false", TokenType::FALSE },
        { "for", TokenType::FOR },
        { "fun", TokenType::FUN },
        { "if", TokenType::IF },
        { "nil", TokenType::NIL },
        { "or", TokenType::OR },
        { "print", TokenType::PRINT },
        { "return", TokenType::RETURN },
        { "super", TokenType::SUPER },
        { "this", TokenType::THIS },
        { "true", TokenType::TRUE },
        { "var", TokenType::VAR },
        { "while", TokenType::WHILE }
};


vector<Token_ptr> scan(const string &source) {
    return Scanner{source}.scan();
}

Scanner::Scanner(const string &source) : source{source}, current{source.begin()}, start{source.begin()} {}

vector<Token_ptr> Scanner::scan() {
    while (!isAtEnd()) {
        nextToken();
    }

    tokens.push_back( Token::New(TokenType::END_OF_FILE, "", line) );
    return move(tokens);
}

void Scanner::nextToken() {
    start = current;

    char c = advance();

    switch(c) {

        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            // Ignore
            break;

        // Newline
        case '\n':
            line++;
            break;

        // Single-character tokens
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;

        // Single or double-character tokens
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

        case '/':
            if (match('/')) {
                // Comments
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }

            } else {
                // Slash operator
                addToken(TokenType::SLASH);
            }

            break;

        // Strings
        case '"': scanString(); break;

        default:
            if (isDigit(c)) {
                // Numeric literal
                scanNumber();

            } else if (isAlpha(c)) {
                // Identifier
                scanIdentifier();

            } else {
                throw ScanError(line, ErrorType::UNEXPECTED_CHARACTER);
            }

            break;
    }
}

void Scanner::scanString() {

    // Scan until closing quotation mark
    while (peek() != '"' && !isAtEnd()) {

        // Handle newlines within the string. As a consequence, Lox supports multi-line strings
        if (peek() == '\n') {
            line++;
        }

        advance();
    }

    // If no closing quotation mark has been found, the string is untermindated
    if (isAtEnd()) {
        throw ScanError(line, ErrorType::UNTERMINATED_STRING);
    }

    // Consume closing quotation mark
    advance();

    addToken(TokenType::STRING);
}

void Scanner::scanNumber() {

    // Scan the integer part
    while (isDigit(peek())) {
        advance();
    }

    // Look for a decimal point and at least one decimal place
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the decimal point
        advance();

        // Consume the fractional part
        while (isDigit(peek())) {
            advance();
        }
    }

    addToken(TokenType::NUMBER);
}

void Scanner::scanIdentifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    // See if the identifier is a reserved word
    string lexeme {start, current};
    auto type = (keywords.count(lexeme) == 1) ? keywords.at(lexeme) : TokenType::IDENTIFIER;

    addToken(type);
}

char Scanner::advance() {
    ++current;
    return *(current - 1);
}

bool Scanner::match(char expected) {
    // consumes the next character if it matches the argument

    if (peek() != expected) {
        return false;
    } else {
        advance();
        return true;
    }
}

char Scanner::peek() {
    // 1-step lookahead

    if (isAtEnd()) {
        return '\0';
    } else {
        return *current;
    }
}

char Scanner::peekNext() {
    // 2-step lookahead

    if (current + 1 == source.end()) {
        return '\0';
    } else {
        return *(current + 1);
    }
}

void Scanner::addToken(const TokenType &type) {
    tokens.push_back(Token::New(type, string{start, current}, line));
}

bool Scanner::isAtEnd() {
    return current == source.end();
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

bool isAlphaNumeric(char c) {
    return isAlpha(c) || isdigit(c);
}

string ScanError::what() const noexcept {
    string description;

    switch(type) {
        case ErrorType::UNEXPECTED_CHARACTER:
            description = "Unexpected character";
            break;
        case ErrorType::UNTERMINATED_STRING:
            description = "Unterminated string";
            break;
    }

    return report(line, "", description);
}
