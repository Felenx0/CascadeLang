#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "tokenization.hpp"
#include "parse.hpp"
#include "generation.hpp"

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

    Tokenizer tokenizer(move(contents));
    vector<Token> tokens = tokenizer.tokenize();

    Parser parser(move(tokens));
    optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        cerr << "No exit statement found" << endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value()); {
        fstream file("out.asm", ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");
    return EXIT_SUCCESS;
}
