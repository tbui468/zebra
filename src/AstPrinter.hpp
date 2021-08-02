#ifndef ZEBRA_AST_PRINTER_H
#define ZEBRA_AST_PRINTER_H

#include <string>
#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {

    class AstPrinter: public ExprStringVisitor {
        public:
            AstPrinter() {}
            ~AstPrinter() {}
            void print(std::vector<std::shared_ptr<Expr>> ast) {
                for (std::shared_ptr<Expr> expr: ast) {
                    std::cout << to_string(expr.get()) << std::endl;
                }

            }
        private:
            std::string to_string(Expr* expr) {
                return expr->accept(*this); 
            }

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
            std::string visit(VarDecl* expr) override {
                return "VarDecl";
            }
            std::string visit(Assign* expr) override {
                return "Assign";
            }
            std::string visit(Block* expr) override {
                std::string ret = "( Block ";
                for(std::shared_ptr<Expr> e: expr->m_expressions) {
                    ret += e->accept(*this) + ", ";
                }
                return ret + " )";
            }
            std::string visit(If* expr) override {
                std::string ret = "( If " + to_string(expr->m_condition.get()) + " then " + to_string(expr->m_then_branch.get());
                if (expr->m_else_branch) {
                    ret += " else " + to_string(expr->m_else_branch.get());
                }

                return ret + " )";
            }
            std::string visit(For* expr) override {
                return "( For )";
            }
            std::string visit(While* expr) override {
                return "While";
            }
            std::string visit(FunDecl* expr) override {
                return "FunDecl";
            }
            std::string visit(Return* expr) override {
                return "Return";
            }
            std::string visit(Call* expr) override {
                return "Call";
            }
            std::string visit(MethodCall* expr) override {
                return "MethodCall";
            }
            std::string visit(Import* expr) override {
                return "Import";
            }
            std::string visit(ClassDecl* expr) override {
                return "ClassDecl";
            }
            std::string visit(InstClass* expr) override {
                return "InstClass";
            }
            std::string visit(GetField* expr) override {
                return "GetField";
            }
            std::string visit(SetField* expr) override {
                return "SetField";
            }

    };


}




#endif // ZEBRA_AST_PRINTER_H
