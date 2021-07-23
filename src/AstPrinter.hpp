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
            void print(std::shared_ptr<Stmt> stmt);
        private:
            virtual std::string visit(std::shared_ptr<Unary> expr) override;
            virtual std::string visit(std::shared_ptr<Binary> expr) override;
            virtual std::string visit(std::shared_ptr<Group> expr) override;
            virtual std::string visit(std::shared_ptr<Literal> expr) override;
            virtual std::string visit(std::shared_ptr<Print> stmt) override;
            virtual std::string visit(std::shared_ptr<If> stmt) override;
            virtual std::string visit(std::shared_ptr<Block> stmt) override;
    };


}




#endif // ZEBRA_AST_PRINTER_H
