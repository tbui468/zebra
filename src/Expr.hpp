#ifndef ZEBRA_EXPR_H
#define ZEBRA_EXPR_H

#include "Token.hpp"

namespace zebra {

    //Expressions
    struct Expr {
        public:
            virtual ~Expr() {}
    };

    struct Binary: public Expr {
        public:
            Binary(Token op, Expr* left, Expr* right): m_op(op), m_left(left), m_right(right) {}
            ~Binary() {
                delete m_left;
                delete m_right;
            }
        public:
            Token m_op;
            Expr* m_left;
            Expr* m_right;
    };

    struct Unary: public Expr {
        public:
            Unary(Token op, Expr* right): m_op(op), m_right(right) {}
            ~Unary() { delete m_right; }
        public:
            Token m_op;
            Expr* m_right;
    };

    struct Group: public Expr {
        public:
            Group(Token name, Expr* expr): m_name(name), m_expr(expr) {}
            ~Group() { delete m_expr; }
        public:
            Token m_name;
            Expr* m_expr;
    };

    struct Literal: public Expr {
        public:
            Literal(Token token): m_token(token) {}
            ~Literal() {}
        public:
            Token m_token;
    };

}


#endif //ZEBRA_EXPR_H
