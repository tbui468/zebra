#ifndef ZEBRA_EXPR_H
#define ZEBRA_EXPR_H

#include <string>
#include <memory>
#include <unordered_map>
#include "Token.hpp"

namespace zebra {

    struct Stmt;
    class Object;

    struct Unary;
    struct Binary;
    struct Group;
    struct Literal;
    struct Logic;
    struct Variable;
    struct StmtExpr;
    struct Access;
    struct VarDecl;
    struct Assign;
    struct Block;
    struct If;
    struct For;
    struct While;
    struct FunDecl;
    struct Return;
    struct Call;
    struct Import;
    struct ClassDecl;
    struct InstClass;

    struct ExprStringVisitor {
        virtual std::string visit(Unary* expr) = 0;
        virtual std::string visit(Binary* expr) = 0;
        virtual std::string visit(Group* expr) = 0;
        virtual std::string visit(Literal* expr) = 0;
        virtual std::string visit(Logic* expr) = 0;
        virtual std::string visit(Variable* expr) = 0;
        virtual std::string visit(StmtExpr* expr) = 0;
        virtual std::string visit(Access* expr) = 0;
        virtual std::string visit(VarDecl* expr) = 0;
        virtual std::string visit(Assign* expr) = 0;
        virtual std::string visit(Block* expr) = 0;
        virtual std::string visit(If* expr) = 0;
        virtual std::string visit(For* expr) = 0;
        virtual std::string visit(While* expr) = 0;
        virtual std::string visit(FunDecl* expr) = 0;
        virtual std::string visit(Return* expr) = 0;
        virtual std::string visit(Call* expr) = 0;
        virtual std::string visit(Import* expr) = 0;
        virtual std::string visit(ClassDecl* expr) = 0;
        virtual std::string visit(InstClass* expr) = 0;
    };

    struct ExprObjectVisitor {
        virtual std::shared_ptr<Object> visit(Unary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Binary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Group* expr) = 0;
        virtual std::shared_ptr<Object> visit(Literal* expr) = 0;
        virtual std::shared_ptr<Object> visit(Logic* expr) = 0;
        virtual std::shared_ptr<Object> visit(Variable* expr) = 0;
        virtual std::shared_ptr<Object> visit(StmtExpr* expr) = 0;
        virtual std::shared_ptr<Object> visit(Access* expr) = 0;
        virtual std::shared_ptr<Object> visit(VarDecl* expr) = 0;
        virtual std::shared_ptr<Object> visit(Assign* expr) = 0;
        virtual std::shared_ptr<Object> visit(Block* expr) = 0;
        virtual std::shared_ptr<Object> visit(If* expr) = 0;
        virtual std::shared_ptr<Object> visit(For* expr) = 0;
        virtual std::shared_ptr<Object> visit(While* expr) = 0;
        virtual std::shared_ptr<Object> visit(FunDecl* expr) = 0;
        virtual std::shared_ptr<Object> visit(Return* expr) = 0;
        virtual std::shared_ptr<Object> visit(Call* expr) = 0;
        virtual std::shared_ptr<Object> visit(Import* expr) = 0;
        virtual std::shared_ptr<Object> visit(ClassDecl* expr) = 0;
        virtual std::shared_ptr<Object> visit(InstClass* expr) = 0;
    };

    struct ExprTokenTypeVisitor {
        virtual TokenType visit(Unary* expr) = 0;
        virtual TokenType visit(Binary* expr) = 0;
        virtual TokenType visit(Group* expr) = 0;
        virtual TokenType visit(Literal* expr) = 0;
        virtual TokenType visit(Logic* expr) = 0;
        virtual TokenType visit(Variable* expr) = 0;
        virtual TokenType visit(StmtExpr* expr) = 0;
        virtual TokenType visit(Access* expr) = 0;
        virtual TokenType visit(VarDecl* expr) = 0;
        virtual TokenType visit(Assign* expr) = 0;
        virtual TokenType visit(Block* expr) = 0;
        virtual TokenType visit(If* expr) = 0;
        virtual TokenType visit(For* expr) = 0;
        virtual TokenType visit(While* expr) = 0;
        virtual TokenType visit(FunDecl* expr) = 0;
        virtual TokenType visit(Return* expr) = 0;
        virtual TokenType visit(Call* expr) = 0;
        virtual TokenType visit(Import* expr) = 0;
        virtual TokenType visit(ClassDecl* expr) = 0;
        virtual TokenType visit(InstClass* expr) = 0;
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

    struct StmtExpr: public Expr {
        public:
            StmtExpr(std::shared_ptr<Stmt> stmt): m_stmt(stmt) {}
            ~StmtExpr() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Stmt> m_stmt;
    };

    struct Access: public Expr {
        public:
            Access(Token instance, Token field): m_instance(instance), m_field(field) {}
            ~Access() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_instance;
            Token m_field;
    };


    struct VarDecl: public Expr {
        public:
            VarDecl(Token name, std::shared_ptr<Expr> value): 
                m_name(name), m_value(value) {}
            ~VarDecl() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };

    struct Assign: public Expr {
        public:
            Assign(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~Assign() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };


    struct Block: public Expr {
        public:
            Block(Token name, std::vector<std::shared_ptr<Expr>> expressions): m_name(name), m_expressions(expressions) {}
            ~Block() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_expressions;
    };

    struct If: public Expr {
        public:
            If(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> then_branch, std::shared_ptr<Expr> else_branch): 
                m_name(name), m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_then_branch;
            std::shared_ptr<Expr> m_else_branch;
    };

    struct For: public Expr {
        public:
            For(Token name, std::shared_ptr<Expr> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> update, std::shared_ptr<Expr> body): 
                m_name(name), m_initializer(initializer), m_condition(condition), m_update(update), m_body(body) {}
            ~For() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_initializer;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_update;
            std::shared_ptr<Expr> m_body;
    };


    struct While: public Expr {
        public:
            While(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> body): 
                m_name(name), m_condition(condition), m_body(body) {}
            ~While() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_body;
    };

    struct FunDecl: public Expr {
        public:
            FunDecl(Token name, std::vector<std::shared_ptr<Expr>> parameters, TokenType type, std::shared_ptr<Expr> body): 
                m_name(name), m_parameters(parameters), m_return_type(type), m_body(body), m_arity(parameters.size()) {}
            ~FunDecl() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_parameters;
            TokenType m_return_type;
            std::shared_ptr<Expr> m_body;
            int m_arity;
    };

    struct Return: public Expr {
        public:
            Return(Token name, TokenType return_type, std::shared_ptr<Expr> value): 
                m_name(name), m_return_type(return_type), m_value(value) {}
            ~Return() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            TokenType m_return_type;
            std::shared_ptr<Expr> m_value;
    };

    struct Call: public Expr {
        public:
            Call(Token name, std::vector<std::shared_ptr<Expr>> arguments): 
                m_name(name), m_arguments(arguments), m_arity(arguments.size()) {}
            ~Call() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
            std::shared_ptr<Object> m_return {nullptr};
    };

    struct Import: public Expr {
        public:
            Import(Token name, std::unordered_map<std::string, std::shared_ptr<Object>> functions):
                m_name(name), m_functions(functions) {}
            ~Import() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::unordered_map<std::string, std::shared_ptr<Object>> m_functions;
    };

    struct ClassDecl: public Expr {
        public:
            ClassDecl(Token name, std::vector<std::shared_ptr<Expr>> fields):
                m_name(name), m_fields(fields) {}
            ~ClassDecl() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_fields;
    };

    struct InstClass: public Expr {
        public:
            InstClass(Token name, Token class_decl, std::vector<std::shared_ptr<Expr>> arguments):
                m_name(name), m_class(class_decl), m_arguments(arguments) {}
            ~InstClass() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            TokenType accept(ExprTokenTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_class;
            std::vector<std::shared_ptr<Expr>> m_arguments;
    };


}


#endif //ZEBRA_EXPR_H
