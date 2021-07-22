#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include "Expr.hpp"

namespace zebra {

    struct Print;

    struct StmtStringVisitor {
        virtual const char* visit(Print& stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual const char* accept(StmtStringVisitor& visitor) = 0;
    };

    struct Print: public Stmt {
        public:
            Print(Expr* value): m_value(value) {}
            ~Print() { delete m_value; }
            virtual const char* accept(StmtStringVisitor& visitor) override { return visitor.visit(*this); }
        public:
            Expr* m_value;
    };


}


#endif //ZEBRA_STMT_H
