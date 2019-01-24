#include "scanner.h"
#include "token.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void runFile(const string &path);
void runPrompt();
void run(const string &source);


int main(int argc, char *argv[]) {

    if (argc == 1) {
        runPrompt();
    } else if (argc == 2) {
        runFile(string {argv[1]});
    } else  {
        cout << "Usage: lox [script]\n";
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void runFile(const string &path) {
    ifstream file {path};
    string input {istreambuf_iterator<char> {file}, istreambuf_iterator<char> {}};

    try {
        run(input);
    } catch (const SyntaxError &e) {
        exit(EXIT_FAILURE);
    }
}

void runPrompt() {
    while (true) {
        string input;

        cout << "> ";

        getline(cin, input);

        try {
            run(input);
        } catch (const SyntaxError &e) {
            continue;
        }
    }
}


void run(const string &source) {
    vector<Token> tokens;

    try {
        tokens = scan(source);
    } catch (const SyntaxError &e) {
        cerr << e.what() << "\n";
        throw;
    }

    for (const auto &token : tokens) {
        cout << token << "\n";
    }
}