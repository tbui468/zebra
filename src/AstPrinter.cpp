#include <iostream>
#include "AstPrinter.hpp"


namespace zebra {


    void AstPrinter::print(Stmt& stmt) {
        std::cout << stmt.accept(*this) << std::endl;
    }

    std::string AstPrinter::visit(Unary& expr) {
        return "( " + expr.m_op.to_string() + " " + expr.m_right->accept(*this);
    }
    std::string AstPrinter::visit(Binary& expr) {
        return "( " + expr.m_op.to_string() + " " + expr.m_left->accept(*this) + " " + expr.m_right->accept(*this) + " )";
    }
    std::string AstPrinter::visit(Group& expr) {
        return "( " + expr.m_expr->accept(*this) + " )";
    }
    std::string AstPrinter::visit(Literal& expr) {
        return expr.m_token.to_string();
    }
    std::string AstPrinter::visit(Print& stmt) {
        return "( Print " + stmt.m_value->accept(*this) + " )";
    }
    std::string AstPrinter::visit(If& stmt) {
        return "If";
       // return "( If " + stmt.m_value->accept(*this) + " )";
    }
    std::string AstPrinter::visit(Block& stmt) {
        return "Block";
        //return "( Block " + stmt.m_value->accept(*this) + " )";
    }


}
