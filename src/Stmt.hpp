#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include "Expr.hpp"

namespace zebra {

    struct Print;

    struct StmtStringVisitor {
        virtual std::string visit(Print& stmt) = 0;
    };


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
    };

    struct Print: public Stmt {
        public:
            Print(Expr* value): m_value(value) {}
            ~Print() { delete m_value; }
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(*this); }
        public:
            Expr* m_value;
    };


}


#endif //ZEBRA_STMT_H
