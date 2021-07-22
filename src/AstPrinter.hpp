#ifndef ZEBRA_AST_PRINTER_H
#define ZEBRA_AST_PRINTER_H

#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {
/*
    class AstPrinter: public ExprStringVisitor, public StmtStringVisitor {
        public:
            AstPrinter() {}
            ~AstPrinter() {}
            void print(Stmt& stmt);
        private:
            virtual const char* visit(Unary& expr) override;
            virtual const char* visit(Binary& expr) override;
            virtual const char* visit(Group& expr) override;
            virtual const char* visit(Literal& expr) override;
            virtual const char* visit(Print& stmt) override;
    };
*/

}




#endif // ZEBRA_AST_PRINTER_H
