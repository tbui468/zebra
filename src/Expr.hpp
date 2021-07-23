#ifndef ZEBRA_EXPR_H
#define ZEBRA_EXPR_H

#include <string>
#include <memory>
#include "Token.hpp"
#include "Object.hpp"

namespace zebra {

    struct Unary;
    struct Binary;
    struct Group;
    struct Literal;
    struct Logic;

    struct ExprStringVisitor {
        virtual std::string visit(std::shared_ptr<Unary> expr) = 0;
        virtual std::string visit(std::shared_ptr<Binary> expr) = 0;
        virtual std::string visit(std::shared_ptr<Group> expr) = 0;
        virtual std::string visit(std::shared_ptr<Literal> expr) = 0;
        virtual std::string visit(std::shared_ptr<Logic> expr) = 0;
    };

    struct ExprObjectVisitor {
        virtual std::shared_ptr<Object> visit(std::shared_ptr<Unary> expr) = 0;
        virtual std::shared_ptr<Object> visit(std::shared_ptr<Binary> expr) = 0;
        virtual std::shared_ptr<Object> visit(std::shared_ptr<Group> expr) = 0;
        virtual std::shared_ptr<Object> visit(std::shared_ptr<Literal> expr) = 0;
        virtual std::shared_ptr<Object> visit(std::shared_ptr<Logic> expr) = 0;
    };

    //Expressions
    struct Expr {
        public:
            virtual ~Expr() {}
            virtual std::string accept(ExprStringVisitor& visitor) = 0;
            virtual std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) = 0;
    };


    struct Unary: public Expr, public std::enable_shared_from_this<Unary> {
        public:
            Unary(Token op, std::shared_ptr<Expr> right): m_op(op), m_right(right) {}
            ~Unary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_right;
    };

    struct Binary: public Expr, public std::enable_shared_from_this<Binary> {
        public:
            Binary(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Binary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };


    struct Group: public Expr, public std::enable_shared_from_this<Group> {
        public:
            Group(Token name, std::shared_ptr<Expr> expr): m_name(name), m_expr(expr) {}
            ~Group() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_expr;
    };

    struct Literal: public Expr, public std::enable_shared_from_this<Literal> {
        public:
            Literal(Token token): m_token(token) {}
            ~Literal() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            Token m_token;
    };


    struct Logic: public Expr, public std::enable_shared_from_this<Logic> {
        public:
            Logic(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Logic() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };

}


#endif //ZEBRA_EXPR_H
