#pragma once

#include <variant>

struct NodeExprIntLit {
    Token int_lit;
};

struct NodeExprIdent {
    Token ident;
};

struct NodeExpr {
    variant<NodeExprIntLit, NodeExprIdent> var;
};

struct NodeExit {
    NodeExpr expr;
};

struct NodeStmtExit {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr expr;
};

struct NodeStmt {
    variant<NodeStmtExit, NodeStmtLet> var;
};

struct NodeProg {
    vector<NodeStmt> stmts;
};

class Parser {
public:
    inline explicit Parser(vector<Token> tokens) : m_tokens(move(tokens)) {}

    optional<NodeExpr> parse_expr() {
        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            return NodeExpr {.var = NodeExprIntLit {.int_lit = consume()}};
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident) {
            return NodeExpr {.var = NodeExprIdent {.ident = consume()}};
        }
        else {
            return {};
        }
    }

    optional<NodeStmt> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::_exit) {
            consume();
            if (peek().has_value() && peek().value().type == TokenType::open_param) {
                consume();

                NodeStmtExit stmt_exit;

                if (auto node_expr = parse_expr()) {
                    stmt_exit = {.expr = node_expr.value()};
                }
                else {
                    cerr << "Invalid Expression" << endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == TokenType::close_param) {
                    consume();
                }
                else {
                    cerr << "Expected `)`" << endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == TokenType::semi) {
                    consume();
                }
                else {
                    cerr << "Expected `;`" << endl;
                    exit(EXIT_FAILURE);
                }

                return NodeStmt {.var = stmt_exit};
            }
            else {
                cerr << "Expected `(`" << endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (peek().has_value() && peek().value().type == TokenType::let) {
            consume();
            if (peek().has_value() && peek().value().type == TokenType::ident) {
                if (peek(1).has_value() && peek(1).value().type == TokenType::eq) {
                    auto stmt_let = NodeStmtLet{.ident = consume()};
                    consume();

                    if (auto node_expr = parse_expr()) {
                        stmt_let.expr = node_expr.value();
                    } else {
                        cerr << "Invalid expression" << endl;
                        exit(EXIT_FAILURE);
                    }

                    if (peek().has_value() && peek().value().type == TokenType::semi) {
                        consume();
                    } else {
                        cerr << "Expected `;`" << endl;
                        exit(EXIT_FAILURE);
                    }

                    return NodeStmt{.var = stmt_let};
                }
                else {
                    cerr << "Expected `=`" << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else {
                cerr << "Expected identifier" << endl;
                exit(EXIT_FAILURE);
            }
        }
        else {
            return {};
        }
    }

    optional<NodeProg> parse_prog() {
        NodeProg prog;

        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            }
            else {
                cerr << "Invalid statement" << endl;
                exit(EXIT_FAILURE);
            }
        }

        return prog;
    }
private:
    [[nodiscard]] inline optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    const vector<Token> m_tokens;
    size_t m_index = 0;
};