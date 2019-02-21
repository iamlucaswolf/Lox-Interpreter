#include "scanner/scanner.h"
#include "data/token.h"
#include "data/ast.h"
#include "utility/ast-tools.h"
#include "parser/parser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void runFile(const string &path);
void runPrompt();
void run(const string &source);


#include <memory>
#include <vector>


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
    } catch (const LoxError &e) {
        cerr << e.what() << "\n";
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
        } catch (const LoxError &e) {
            cerr << e.what() << "\n";
            continue;
        }
    }
}


void run(const string &source) {
    auto tokens = scan(source);
    auto expression = parse(tokens);

    Printer p;
    cout << p.print(*expression) << "\n";
}