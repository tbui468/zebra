#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include "Expr.hpp"

namespace zebra {

    struct Print;
    struct If;
    struct Block;
    struct AssignStmt;
    struct VarDecl;
    struct While;
    struct For;
    struct FunDecl;

    struct StmtStringVisitor {
        virtual std::string visit(Print* stmt) = 0;
        virtual std::string visit(If* stmt) = 0;
        virtual std::string visit(Block* stmt) = 0;
        virtual std::string visit(AssignStmt* stmt) = 0;
        virtual std::string visit(VarDecl* stmt) = 0;
        virtual std::string visit(While* stmt) = 0;
        virtual std::string visit(For* stmt) = 0;
        virtual std::string visit(FunDecl* stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(Print* stmt) = 0;
        virtual void visit(If* stmt) = 0;
        virtual void visit(Block* stmt) = 0;
        virtual void visit(AssignStmt* stmt) = 0;
        virtual void visit(VarDecl* stmt) = 0;
        virtual void visit(While* stmt) = 0;
        virtual void visit(For* stmt) = 0;
        virtual void visit(FunDecl* stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
    };

    struct Print: public Stmt {
        public:
            Print(std::shared_ptr<Expr> value): m_value(value) {}
            ~Print() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
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


    struct AssignStmt: public Stmt {
        public:
            AssignStmt(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~AssignStmt() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
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

    struct FunDecl: public Stmt {
        public:
            FunDecl(Token name, std::vector<std::shared_ptr<Stmt>> arguments, Token ret, std::shared_ptr<Stmt> body): 
                m_name(name), m_arguments(arguments), m_return(ret), m_body(body) {}
            ~FunDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Stmt>> m_arguments;
            Token m_return;
            std::shared_ptr<Stmt> m_body;
    };


}


#endif //ZEBRA_STMT_H
