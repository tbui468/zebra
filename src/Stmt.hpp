#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include "Expr.hpp"

namespace zebra {

    struct Print;
    struct If;
    struct Block;
    struct While;
    struct For;
    struct Return;
//    struct ExprStmt;
    struct Assign;
    struct VarDecl;
    struct FunDecl;
    struct Call;
 //   struct StructDecl;

    struct StmtStringVisitor {
        virtual std::string visit(Print* stmt) = 0;
        virtual std::string visit(If* stmt) = 0;
        virtual std::string visit(Block* stmt) = 0;
        virtual std::string visit(While* stmt) = 0;
        virtual std::string visit(For* stmt) = 0;
        virtual std::string visit(Return* stmt) = 0;
 //       virtual std::string visit(ExprStmt* stmt) = 0;
        virtual std::string visit(Assign* stmt) = 0;
        virtual std::string visit(VarDecl* stmt) = 0;
        virtual std::string visit(FunDecl* stmt) = 0;
        virtual std::string visit(Call* stmt) = 0;
        //virtual std::string visit(StructDecl* stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(Print* stmt) = 0;
        virtual void visit(If* stmt) = 0;
        virtual void visit(Block* stmt) = 0;
        virtual void visit(While* stmt) = 0;
        virtual void visit(For* stmt) = 0;
        virtual void visit(Return* stmt) = 0;
  //      virtual void visit(ExprStmt* stmt) = 0;
        virtual void visit(Assign* stmt) = 0;
        virtual void visit(VarDecl* stmt) = 0;
        virtual void visit(FunDecl* stmt) = 0;
        virtual void visit(Call* stmt) = 0;
        //virtual void visit(StructDecl* stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
    };

    struct Print: public Stmt {
        public:
            Print(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~Print() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };

    struct If: public Stmt {
        public:
            If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch): 
                m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Stmt> m_then_branch;
            std::shared_ptr<Stmt> m_else_branch;
    };

    struct Block: public Stmt {
        public:
            Block(std::vector<std::shared_ptr<Stmt>> statements): m_statements(statements) {}
            ~Block() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::vector<std::shared_ptr<Stmt>> m_statements;
    };



    struct While: public Stmt {
        public:
            While(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body): 
                m_name(name), m_condition(condition), m_body(body) {}
            ~While() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Stmt> m_body;
    };

    struct For: public Stmt {
        public:
            For(Token name, std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> update, std::shared_ptr<Stmt> body): 
                m_name(name), m_initializer(initializer), m_condition(condition), m_update(update), m_body(body) {}
            ~For() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Stmt> m_initializer; //this is messy in parser since statments expect a semicolon
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_update;
            std::shared_ptr<Stmt> m_body;
    };


    struct Return: public Stmt {
        public:
            Return(Token name, Token return_type, std::shared_ptr<Expr> value): m_name(name), m_return_type(return_type), m_value(value) {}
            ~Return() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_return_type;
            std::shared_ptr<Expr> m_value;
    };

    //wrapper for unused expression results
    /*
    struct ExprStmt : public Stmt {
        public:
            ExprStmt(std::shared_ptr<Expr> expr): m_expr(expr) {}
            ~ExprStmt() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Expr> m_expr;
    };*/

    struct Assign: public Stmt {
        public:
            Assign(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~Assign() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };


    struct VarDecl: public Stmt {
        public:
            VarDecl(Token name, Token type, std::shared_ptr<Expr> value): m_name(name), m_type(type), m_value(value) {}
            ~VarDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_type;
            std::shared_ptr<Expr> m_value;
    };


    struct FunDecl: public Stmt {
        public:
            FunDecl(Token name, std::vector<std::shared_ptr<Stmt>> parameters, Token type, std::shared_ptr<Stmt> body): 
                m_name(name), m_parameters(parameters), m_type(type), m_body(body), m_arity(parameters.size()) {}
            ~FunDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Stmt>> m_parameters;
            Token m_type;
            std::shared_ptr<Stmt> m_body;
            int m_arity;
    };


    struct Call: public Stmt {
        public:
            Call(Token name, std::vector<std::shared_ptr<Expr>> arguments): m_name(name), m_arguments(arguments), m_arity(arguments.size()) {}
            ~Call() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
            std::shared_ptr<Object> m_return {nullptr};
    };

/*
    struct StructDecl : public Stmt {
        public:
            StructDecl(std::vector<std::shared_ptr<Stmt>> fields): m_fields(fields) {}
            ~StructDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::vector<std::shared_ptr<Stmt>> m_fields;
    };*/


}


#endif //ZEBRA_STMT_H
