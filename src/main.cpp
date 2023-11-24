#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "tokenization.hpp"

string tokens_to_asm(const vector<Token>& tokens) {
    stringstream output;
    output << "global _start\n_start:\n";

    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::_exit) {
            if (i + 1 < tokens.size() && tokens.at(i+1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i+2).type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i+1).value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }

    return output.str();
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        cerr << "Incorrect usage. Correct usage is.." << endl;
        cerr << "cascade <input.csc>" << endl;
        return EXIT_FAILURE;
    }

    string f_name; { // Saves the file name and type
        string name = argv[1];
        for (char c : name) {
            f_name.push_back(c);

            if (f_name == "../") {
                f_name.clear();
                continue;
            }
        }
    }

    {
        string f_type; // File type
        bool is_dot = false;

        for (char c : f_name) {
            if (c != '.' && !is_dot) {
                continue;
            }
            else if (c == '.') {
                is_dot = true;
                f_type.push_back(c);
                continue;
            }
            else {
                f_type.push_back(c);
                continue;
            }
        }

        if (f_type != ".csc") {
            cerr << "Incorrect usage. Correct usage is.." << endl;
            cerr << "cascade <input.csc>" << endl;
            return EXIT_FAILURE;
        }
    }

    string contents; { // File content
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);
    vector<Token> tokens = tokenizer.tokenize();

    {
        fstream file("out.asm", ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}
