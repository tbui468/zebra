#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include <memory>
#include "Expr.hpp"

namespace zebra {

    struct Print;
    struct If;
    struct Block;

    struct StmtStringVisitor {
        virtual std::string visit(std::shared_ptr<Print> stmt) = 0;
        virtual std::string visit(std::shared_ptr<If> stmt) = 0;
        virtual std::string visit(std::shared_ptr<Block> stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(std::shared_ptr<Print> stmt) = 0;
        virtual void visit(std::shared_ptr<If> stmt) = 0;
        virtual void visit(std::shared_ptr<Block> stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
    };

    struct Print: public Stmt, public std::enable_shared_from_this<Print> {
        public:
            Print(std::shared_ptr<Expr> value): m_value(value) {}
            ~Print() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            std::shared_ptr<Expr> m_value;
    };

    struct If: public Stmt, public std::enable_shared_from_this<If> {
        public:
            If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch): 
                m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Stmt> m_then_branch;
            std::shared_ptr<Stmt> m_else_branch;
    };

    struct Block: public Stmt, public std::enable_shared_from_this<Block> {
        public:
            Block(std::vector<std::shared_ptr<Stmt>> statements): m_statements(statements) {}
            ~Block() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(shared_from_this()); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(shared_from_this()); }
        public:
            std::vector<std::shared_ptr<Stmt>> m_statements;
    };


}


#endif //ZEBRA_STMT_H
