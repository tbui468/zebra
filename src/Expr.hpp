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
    struct AssignExpr;
    struct Variable;
    struct Call;

    struct ExprStringVisitor {
        virtual std::string visit(Unary* expr) = 0;
        virtual std::string visit(Binary* expr) = 0;
        virtual std::string visit(Group* expr) = 0;
        virtual std::string visit(Literal* expr) = 0;
        virtual std::string visit(Logic* expr) = 0;
        virtual std::string visit(AssignExpr* expr) = 0;
        virtual std::string visit(Variable* expr) = 0;
        virtual std::string visit(Call* expr) = 0;
    };

    struct ExprObjectVisitor {
        virtual std::shared_ptr<Object> visit(Unary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Binary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Group* expr) = 0;
        virtual std::shared_ptr<Object> visit(Literal* expr) = 0;
        virtual std::shared_ptr<Object> visit(Logic* expr) = 0;
        virtual std::shared_ptr<Object> visit(AssignExpr* expr) = 0;
        virtual std::shared_ptr<Object> visit(Variable* expr) = 0;
        virtual std::shared_ptr<Object> visit(Call* expr) = 0;
    };

    struct ExprTokenTypeVisitor {
        virtual TokenType visit(Unary* expr) = 0;
        virtual TokenType visit(Binary* expr) = 0;
        virtual TokenType visit(Group* expr) = 0;
        virtual TokenType visit(Literal* expr) = 0;
        virtual TokenType visit(Logic* expr) = 0;
        virtual TokenType visit(AssignExpr* expr) = 0;
        virtual TokenType visit(Variable* expr) = 0;
        virtual TokenType visit(Call* expr) = 0;
    };

    //Expressions
    struct Expr {
        public:
            virtual ~Expr() {}
            virtual std::string accept(ExprStringVisitor& visitor) = 0;
            virtual std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) = 0;
            virtual TokenType accept(ExprTokenTypeVisitor& visitor) = 0;
    };


    struct Unary: public Expr {
        public:
            Unary(Token op, std::shared_ptr<Expr> right): m_op(op), m_right(right) {}
            ~Unary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_right;
    };

    struct Binary: public Expr {
        public:
            Binary(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Binary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };


    struct Group: public Expr {
        public:
            Group(Token name, std::shared_ptr<Expr> expr): m_name(name), m_expr(expr) {}
            ~Group() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_expr;
    };

    struct Literal: public Expr {
        public:
            Literal(Token token): m_token(token) {}
            ~Literal() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_token;
    };


    struct Logic: public Expr {
        public:
            Logic(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Logic() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };

    struct Variable: public Expr {
        public:
            Variable(Token name): m_name(name) {}
            ~Variable() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
    };

    struct AssignExpr: public Expr {
        public:
            AssignExpr(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~AssignExpr() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };


    struct Call: public Expr {
        public:
            Call(Token name, std::vector<std::shared_ptr<Expr>> arguments): m_name(name), m_arguments(arguments), m_arity(arguments.size()) {}
            ~Call() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
    };

}


#endif //ZEBRA_EXPR_H
