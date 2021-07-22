#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include "Expr.hpp"

namespace zebra {

    struct Print;
    struct If;
    struct Block;

    struct StmtStringVisitor {
        virtual std::string visit(Print& stmt) = 0;
        virtual std::string visit(If& stmt) = 0;
        virtual std::string visit(Block& stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(Print& stmt) = 0;
        virtual void visit(If& stmt) = 0;
        virtual void visit(Block& stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
    };

    struct Print: public Stmt {
        public:
            Print(Expr* value): m_value(value) {}
            ~Print() { delete m_value; }
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(*this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(*this); }
        public:
            Expr* m_value;
    };

    struct If: public Stmt {
        public:
            If(Expr* condition, Stmt* then_branch, Stmt* else_branch): 
                m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {
                delete m_condition;
                delete m_then_branch;
                delete m_else_branch;
            }
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(*this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(*this); }
        public:
            Expr* m_condition;
            Stmt* m_then_branch;
            Stmt* m_else_branch;
    };

    struct Block: public Stmt {
        public:
            Block(std::vector<Stmt*> statements): m_statements(statements) {}
            ~Block() {
                for(Stmt* s: m_statements) {
                    delete s;
                }
            }
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(*this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(*this); }
        public:
            std::vector<Stmt*> m_statements;
    };


}


#endif //ZEBRA_STMT_H
