#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include "Expr.hpp"

namespace zebra {


    //Statements
    struct Stmt {
        virtual ~Stmt() {}
    };

    struct Print: public Stmt {
        public:
            Print(Expr* value): m_value(value) {}
            ~Print() { delete m_value; }
        public:
            Expr* m_value;
    };


}


#endif //ZEBRA_STMT_H
