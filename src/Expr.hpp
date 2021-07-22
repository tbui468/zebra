#ifndef ZEBRA_EXPR_H
#define ZEBRA_EXPR_H

#include "Token.hpp"

namespace zebra {

    struct Unary;
    struct Binary;
    struct Group;
    struct Literal;

    struct ExprStringVisitor {
        virtual const char* visit(Unary& expr) = 0;
        virtual const char* visit(Binary& expr) = 0;
        virtual const char* visit(Group& expr) = 0;
        virtual const char* visit(Literal& expr) = 0;
    };

    //Expressions
    struct Expr {
        public:
            virtual ~Expr() {}
            virtual const char* accept(ExprStringVisitor& visitor) = 0;
    };


    struct Unary: public Expr {
        public:
            Unary(Token op, Expr* right): m_op(op), m_right(right) {}
            ~Unary() { delete m_right; }
            const char* accept(ExprStringVisitor& visitor) { return visitor.visit(*this); }
        public:
            Token m_op;
            Expr* m_right;
    };

    struct Binary: public Expr {
        public:
            Binary(Token op, Expr* left, Expr* right): m_op(op), m_left(left), m_right(right) {}
            ~Binary() {
                delete m_left;
                delete m_right;
            }
            const char* accept(ExprStringVisitor& visitor) { return visitor.visit(*this); }
        public:
            Token m_op;
            Expr* m_left;
            Expr* m_right;
    };


    struct Group: public Expr {
        public:
            Group(Token name, Expr* expr): m_name(name), m_expr(expr) {}
            ~Group() { delete m_expr; }
            const char* accept(ExprStringVisitor& visitor) { return visitor.visit(*this); }
        public:
            Token m_name;
            Expr* m_expr;
    };

    struct Literal: public Expr {
        public:
            Literal(Token token): m_token(token) {}
            ~Literal() {}
            const char* accept(ExprStringVisitor& visitor) { return visitor.visit(*this); }
        public:
            Token m_token;
    };

}


#endif //ZEBRA_EXPR_H
