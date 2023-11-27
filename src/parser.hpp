#pragma once

#include <variant>
#include "area.hpp"

struct NodeTermIntLit {
    Token int_lit;
};

struct NodeTermIdent {
    Token ident;
};

struct NodeExpr;

struct NodeBinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

/*struct NodeBinExprMulti {
    NodeExpr* lhs;
    NodeExpr* rhs;
};*/

struct NodeBinExpr {
    NodeBinExprAdd* add;
};

struct NodeTerm {
    variant<NodeTermIntLit*, NodeTermIdent*> var;
};

struct NodeExpr {
    variant<NodeTerm*, NodeBinExpr*> var;
};

struct NodeStmtExit {
    NodeExpr* expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt {
    variant<NodeStmtExit*, NodeStmtLet*> var;
};

struct NodeProg {
    vector<NodeStmt*> stmts;
};

class Parser {
public:
    inline explicit Parser(vector<Token> tokens) :
        m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4) // 4mb
    {
    }

    optional<NodeTerm*> parse_term() {
        if (auto int_lit = try_consume(TokenType::int_lit)) {
            auto* term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto* term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        }
        else if (auto ident = try_consume(TokenType::ident)) {
            auto* term_ident = m_allocator.alloc<NodeTermIdent>();
            term_ident->ident = ident.value();
            auto* term = m_allocator.alloc<NodeTerm>();
            term->var = term_ident;
            return term;
        }
        else {
            return {};
        }
    }

    optional<NodeExpr*> parse_expr() {
        if (auto term = parse_term()) {
            if (try_consume(TokenType::plus).has_value()) {
                auto* bin_expr = m_allocator.alloc<NodeBinExpr>();
                auto* bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
                auto* lhs_expr = m_allocator.alloc<NodeExpr>();
                lhs_expr->var = term.value();
                bin_expr_add->lhs = lhs_expr;
                if (auto rhs = parse_expr()) {
                    bin_expr_add->rhs = rhs.value();
                    bin_expr->add = bin_expr_add;
                    auto expr = m_allocator.alloc<NodeExpr>();
                    expr->var = bin_expr;
                    return expr;
                }
                else {
                    cerr << "Expected expression" << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else {
                auto expr = m_allocator.alloc<NodeExpr>();
                expr->var = term.value();
                return expr;
            }
        }
        else {
            return {};
        }
    }

    optional<NodeStmt*> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::_exit) {
            consume();
            if (peek().has_value() && peek().value().type == TokenType::open_param) {
                consume();
                auto* stmt_exit = m_allocator.alloc<NodeStmtExit>();

                if (auto node_expr = parse_expr()) {
                    stmt_exit->expr = node_expr.value();
                }
                else {
                    cerr << "Invalid Expression" << endl;
                    exit(EXIT_FAILURE);
                }

                try_consume(TokenType::close_param, "Expected `)`");
                try_consume(TokenType::semi, "Expected `;`");

                auto stmt = m_allocator.alloc<NodeStmt>();
                stmt->var = stmt_exit;
                return stmt;
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
                    auto* stmt_let = m_allocator.alloc<NodeStmtLet>();
                    stmt_let->ident = consume();
                    consume();

                    if (auto node_expr = parse_expr()) {
                        stmt_let->expr = node_expr.value();
                    } else {
                        cerr << "Invalid expression" << endl;
                        exit(EXIT_FAILURE);
                    }

                    try_consume(TokenType::semi, "Expected `;`");

                    auto stmt = m_allocator.alloc<NodeStmt>();
                    stmt->var = stmt_let;
                    return stmt;
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

    inline optional<Token> try_consume(TokenType type) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        else {
            return {};
        }
    }

    inline optional<Token> try_consume(TokenType type, const string& err_msg) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        else {
            cerr << err_msg << endl;
            exit(EXIT_FAILURE);
        }
    }

    const vector<Token> m_tokens;
    size_t m_index = 0;
    AreaAllocator m_allocator;
};