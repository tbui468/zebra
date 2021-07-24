#include <iostream>
#include "AstPrinter.hpp"


namespace zebra {


    void AstPrinter::print(Stmt* stmt) {
        std::cout << stmt->accept(*this) << std::endl;
    }

    /*
     * Expressions
     */
    std::string AstPrinter::visit(Unary* expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_right->accept(*this);
    }
    std::string AstPrinter::visit(Binary* expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
    }
    std::string AstPrinter::visit(Group* expr) {
        return "( " + expr->m_expr->accept(*this) + " )";
    }
    std::string AstPrinter::visit(Literal* expr) {
        return "( " + expr->m_token.to_string() + " )";
    }
    std::string AstPrinter::visit(Logic* expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
    }
    std::string AstPrinter::visit(AssignExpr* expr) {
        return "AssignExpr";
    }
    std::string AstPrinter::visit(Variable* expr) {
        return "( " + expr->m_name.to_string() + " )";
    }

    /*
     * Statements
     */
    std::string AstPrinter::visit(Print* stmt) {
        return "( Print " + stmt->m_value->accept(*this) + " )";
    }
    std::string AstPrinter::visit(If* stmt) {
        std::string ret = "( If " + stmt->m_condition->accept(*this) + " then " + stmt->m_then_branch->accept(*this);
        if (stmt->m_else_branch) {
            ret += " else " + stmt->m_else_branch->accept(*this);
        }

        return ret + " )";
    }
    std::string AstPrinter::visit(Block* stmt) {
        std::string ret = "( Block ";
        for(std::shared_ptr<Stmt> s: stmt->m_statements) {
            ret += s->accept(*this) + ", ";
        }
        return ret + " )";
    }
    std::string AstPrinter::visit(AssignStmt* expr) {
        return "AssignStmt";
    }
    std::string AstPrinter::visit(VarDecl* stmt) {
        return "VarDecl";
    }
    std::string AstPrinter::visit(While* stmt) {
        return "While";
    }


}
