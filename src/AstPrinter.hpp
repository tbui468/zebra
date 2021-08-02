#ifndef ZEBRA_AST_PRINTER_H
#define ZEBRA_AST_PRINTER_H

#include <string>
#include "Expr.hpp"

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
             * Misc.
             */
            std::string visit(Import* expr) override {
                return "Import";
            }

            /*
             * Basic
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

            /*
             * Variables and Functions
             */
            std::string visit(GetVar* expr) override {
                return "( " + expr->m_name.to_string() + " )";
            }
            std::string visit(DeclVar* expr) override {
                return "DeclVar";
            }
            std::string visit(SetVar* expr) override {
                return "SetVar";
            }
            std::string visit(DeclFun* expr) override {
                return "FunDecl";
            }
            std::string visit(CallFun* expr) override {
                return "CallFun";
            }
            std::string visit(Return* expr) override {
                return "Return";
            }

            /*
             * Control Flow
             */
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
            
            /*
             * Classes
             */
            std::string visit(DeclClass* expr) override {
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
            std::string visit(CallMethod* expr) override {
                return "MethodCall";
            }

    };


}




#endif // ZEBRA_AST_PRINTER_H
