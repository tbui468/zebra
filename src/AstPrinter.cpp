#include <iostream>
#include "AstPrinter.hpp"


namespace zebra {


    void AstPrinter::print(std::shared_ptr<Stmt> stmt) {
        std::cout << stmt->accept(*this) << std::endl;
    }

    std::string AstPrinter::visit(std::shared_ptr<Unary> expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_right->accept(*this);
    }
    std::string AstPrinter::visit(std::shared_ptr<Binary> expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<Group> expr) {
        return "( " + expr->m_expr->accept(*this) + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<Literal> expr) {
        return "( " + expr->m_token.to_string() + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<Logic> expr) {
        return "( " + expr->m_op.to_string() + " " + expr->m_left->accept(*this) + " " + expr->m_right->accept(*this) + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<Print> stmt) {
        return "( Print " + stmt->m_value->accept(*this) + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<If> stmt) {
        std::string ret = "( If " + stmt->m_condition->accept(*this) + " then " + stmt->m_then_branch->accept(*this);
        if (stmt->m_else_branch) {
            ret += " else " + stmt->m_else_branch->accept(*this);
        }

        return ret + " )";
    }
    std::string AstPrinter::visit(std::shared_ptr<Block> stmt) {
        std::string ret = "( Block ";
        for(std::shared_ptr<Stmt> s: stmt->m_statements) {
            ret += s->accept(*this) + ", ";
        }
        return ret + " )";
    }


}
