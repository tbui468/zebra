#ifndef ZEBRA_AST_PRINTER_H
#define ZEBRA_AST_PRINTER_H

#include <string>
#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {

    class AstPrinter: public ExprStringVisitor, public StmtStringVisitor {
        public:
            AstPrinter() {}
            ~AstPrinter() {}
            void print(Stmt* stmt);
        private:
            //expressions
            virtual std::string visit(Unary* expr) override;
            virtual std::string visit(Binary* expr) override;
            virtual std::string visit(Group* expr) override;
            virtual std::string visit(Literal* expr) override;
            virtual std::string visit(Logic* expr) override;
            virtual std::string visit(AssignExpr* expr) override;
            virtual std::string visit(Variable* expr) override;
            //statements
            virtual std::string visit(Print* stmt) override;
            virtual std::string visit(If* stmt) override;
            virtual std::string visit(Block* stmt) override;
            virtual std::string visit(AssignStmt* expr) override;
            virtual std::string visit(VarDecl* stmt) override;
            virtual std::string visit(While* stmt) override;
    };


}




#endif // ZEBRA_AST_PRINTER_H
