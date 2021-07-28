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
            void print(Stmt* stmt) {
                std::cout << stmt->accept(*this) << std::endl;
            }
        private:

            /*
             * Expressions
             */
            std::string visit(Unary* expr) override {
                return "( " + expr->m_op.to_string() + " " + expr->m_right->accept(*this);
            }
            std::string visit(Binary* expr) override {
                return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
            }
            std::string visit(Group* expr) override {
                return "( " + expr->m_expr->accept(*this) + " )";
            }
            std::string visit(Literal* expr) override {
                return "( " + expr->m_token.to_string() + " )";
            }
            std::string visit(Logic* expr) override {
                return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
            }
            std::string visit(Variable* expr) override {
                return "( " + expr->m_name.to_string() + " )";
            }
            std::string visit(StmtExpr* expr) override {
                return "StmtExpr";
            }
            std::string visit(Access* expr) override {
                return "Access";
            }

            /*
             * Statements
             */
            std::string visit(Print* stmt) override {
                return "( Print " + stmt->m_value->accept(*this) + " )";
            }
            std::string visit(If* stmt) override {
                std::string ret = "( If " + stmt->m_condition->accept(*this) + " then " + stmt->m_then_branch->accept(*this);
                if (stmt->m_else_branch) {
                    ret += " else " + stmt->m_else_branch->accept(*this);
                }

                return ret + " )";
            }
            std::string visit(Block* stmt) override {
                std::string ret = "( Block ";
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    ret += s->accept(*this) + ", ";
                }
                return ret + " )";
            }
            std::string visit(While* stmt) override {
                return "While";
            }
            std::string visit(For* stmt) override {
                return "For";
            }
            std::string visit(Assign* stmt) override {
                return "Assign";
            }
            std::string visit(AssignField* stmt) override {
                return "Assign";
            }
            std::string visit(VarDecl* stmt) override {
                return "VarDecl";
            }
            std::string visit(FunDecl* stmt) override {
                return "FunDecl";
            }
            std::string visit(Return* stmt) override {
                return "Return";
            }

            std::string visit(Call* stmt) override {
                return "Call";
            }
            
            std::string visit(StructDecl* stmt) override {
                return "StructDecl";
            }
            std::string visit(StructInst* stmt) override {
                return "StructInst";
            }
    };


}




#endif // ZEBRA_AST_PRINTER_H
