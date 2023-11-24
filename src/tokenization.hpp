#pragma once

#include <vector>
#include <optional>

enum TokenType {
    _exit,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    optional<string> value;
};

vector<Token> tokenize(const string& str) {
    string buf;
    vector<Token> tokens;

    for (int i = 0; i < str.size(); i++) {
        char c = str.at(i);
        if (isalpha(c)) {
            buf.push_back(c);
            i++;
            while (isalnum(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            if (buf == "exit") {
                tokens.push_back({.type = TokenType::_exit});
                buf.clear();
            }
            else {
                cerr << "You messed up!" << endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (isdigit(c)) {
            buf.push_back(c);
            i++;
            while (isdigit(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        }
        else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        }
        else if (isspace(c)) {
            continue;
        }
        else {
            cerr << "You messed up!" << endl;
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}