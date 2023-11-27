#pragma once

#include <vector>
#include <optional>

enum TokenType {
    _exit,
    int_lit,
    semi,
    open_param,
    close_param,
    ident,
    let,
    eq,
    plus
};

struct Token {
    TokenType type;
    optional<string> value;
};

class Tokenizer {
public:
    inline explicit Tokenizer(string src) : m_src(std::move(src)) {}

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
                else if (buf == "let") {
                    tokens.push_back({.type = TokenType::let});
                    buf.clear();
                }
                else {
                    tokens.push_back({.type = TokenType::ident, .value = buf});
                    buf.clear();
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
            else if (peek().value() == '(') {
                consume();
                tokens.push_back({.type = TokenType::open_param});
            }
            else if (peek().value() == ')') {
                consume();
                tokens.push_back({.type = TokenType::close_param});
            }
            else if (peek().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::semi});
            }
            else if (peek().value() == '=') {
                consume();
                tokens.push_back({.type = TokenType::eq});
            }
            else if (peek().value() == '+') {
                consume();
                tokens.push_back({.type = TokenType::plus});
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