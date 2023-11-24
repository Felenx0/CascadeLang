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

class Tokenizer {
public:
    inline explicit Tokenizer(string src) : m_src(move(src)) {}

    inline vector<Token> tokenize() {
        string buf;
        vector<Token> tokens;

        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({.type = TokenType::_exit});
                    buf.clear();
                }
                else {
                    cerr << "You messed up!" << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({.type = TokenType::int_lit, .value = buf});
                buf.clear();
            }
            else if (peek().value() == ';') {
                tokens.push_back({.type = TokenType::semi});
                consume();
            }
            else if (isspace(peek().value())) {
                consume();
            }
            else {
                cerr << "You messed up!" << endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }
private:
    [[nodiscard]] inline optional<char> peek(int offset = 1) const {
        if (m_index + offset > m_src.length()) {
            return {};
        }
        else {
            return m_src.at(m_index);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }

    const string m_src;
    size_t m_index = 0;
};