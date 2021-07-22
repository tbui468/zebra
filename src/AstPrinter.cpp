#include <stdio.h>
#include "AstPrinter.hpp"


namespace zebra {

/*
    void AstPrinter::print(Stmt& stmt) {
        printf("%s", stmt.accept(*this)); 
    }

    const char* AstPrinter::visit(Unary& expr) {
        return "( " + expr.m_op.to_string() + " " + expr.m_right->accept(*this);
    }
    const char* AstPrinter::visit(Binary& expr) {
        return "( " + expr.m_op.to_string() + " " + expr.m_left->accept(*this) + " " + expr.m_right->accept(*this) + " )";
    }
    const char* AstPrinter::visit(Group& expr) {
        return "( " + expr.m_expr->accept(*this) + " )";
    }
    const char* AstPrinter::visit(Literal& expr) {
        return expr.m_token.to_string();
    }
    const char* AstPrinter::visit(Print& stmt) {
        return "( Print " + stmt.m_value->accept(*this) + " )";
    }*/

}
