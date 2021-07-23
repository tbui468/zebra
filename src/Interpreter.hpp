#ifndef ZEBRA_INTERPRETER_H
#define ZEBRA_INTERPRETER_H

#include <vector>
#include <iostream>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Object.hpp"

namespace zebra {

    class Interpreter: public StmtVoidVisitor, public ExprObjectVisitor {
        private:
            std::vector<std::shared_ptr<Stmt>> m_statements;
            class RuntimeError {
                private:
                    Token m_token;
                    std::string m_message;
                public:
                    RuntimeError(Token token, const std::string& message): m_token(token), m_message(message){}
                    ~RuntimeError() {}
                    void print() {
                        std::cout << "[Line " << m_token.m_line << "] Error: " << m_message << std::endl;
                    }
            };
        public:
            Interpreter(const std::vector<std::shared_ptr<Stmt>> statements): m_statements(statements) {}
            ~Interpreter() {}
            void run() {
                try {
                    for(std::shared_ptr<Stmt> s: m_statements) {
                        execute(*s);
                    }
                } catch(RuntimeError& e) {
                    e.print();
                }
            }
            void execute(Stmt& stmt) {
                stmt.accept(*this);
            }

            void visit(std::shared_ptr<Print> stmt) {
                std::shared_ptr<Object> value = stmt->m_value->accept(*this);
                if(value->is_string()) {
                    std::cout << value->get_string() << std::endl;
                }else if(value->is_float()) {
                    std::cout << value->get_float() << std::endl;
                }else if(value->is_int()) {
                    std::cout << value->get_int() << std::endl;
                }else if(value->is_bool()) {
                    if(value->is_truthy()) {
                        std::cout << "true" << std::endl;
                    }else {
                        std::cout << "false" << std::endl;
                    }
                }else if(value->is_nil()) {
                    std::cout << "nil" << std::endl;
                }
            }

            void visit(std::shared_ptr<If> stmt) {
                std::shared_ptr<Object> condition = evaluate(stmt->m_condition);
                if(condition->is_truthy()) {
                    execute(*(stmt->m_then_branch));                    
                }else if(stmt->m_else_branch) {
                    execute(*(stmt->m_else_branch));
                }
            }

            void visit(std::shared_ptr<Block> stmt) {
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    execute(*s);  
                }    
            }

            std::shared_ptr<Object> evaluate(std::shared_ptr<Expr> expr) {
                return expr->accept(*this);
            }

            std::shared_ptr<Object> visit(std::shared_ptr<Unary> expr) {
                std::shared_ptr<Object> right = expr->m_right->accept(*this);

                if(expr->m_op.m_type == TokenType::MINUS) {
                    if(right->is_float()) {
                        return std::make_shared<Object>(-right->get_float());
                    }else if(right->is_int()) {
                        return std::make_shared<Object>(-right->get_int());
                    }
                }else if(expr->m_op.m_type == TokenType::BANG) {
                    return std::make_shared<Object>(!right->is_truthy());
                }

                throw RuntimeError(expr->m_op, "That invalid unary operator.");
            }

            std::shared_ptr<Object> visit(std::shared_ptr<Binary> expr) {
                //if left and right aren't of the same type (string, float or int), throw error
                std::shared_ptr<Object> left = expr->m_left->accept(*this);
                std::shared_ptr<Object> right = expr->m_right->accept(*this);
                if(left->is_int() && right->is_int()) {
                    int a = left->get_int();
                    int b = right->get_int();
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Object>(a + b);
                        case TokenType::MINUS: return std::make_shared<Object>(a - b);
                        case TokenType::STAR: return std::make_shared<Object>(a * b);
                        case TokenType::SLASH: return std::make_shared<Object>(a / b);
                        case TokenType::MOD: return std::make_shared<Object>(a % b);
                    }
                    throw RuntimeError(expr->m_op, "That operation can't be performed on ints.");
                }
                if(left->is_float() && right->is_float()) {
                    float a = left->get_float();
                    float b = right->get_float();
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Object>(a + b);
                        case TokenType::MINUS: return std::make_shared<Object>(a - b);
                        case TokenType::STAR: return std::make_shared<Object>(a * b);
                        case TokenType::SLASH: return std::make_shared<Object>(a / b);
                    }
                    throw RuntimeError(expr->m_op, "That operation can't be performed on floats.");
                }
                if(left->is_string() && right->is_string()) {
                    std::string a = left->get_string();
                    std::string b = right->get_string();
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Object>(a + b);
                    }
                    throw RuntimeError(expr->m_op, "That operation can't be performed on strings.");
                }

                throw RuntimeError(expr->m_op, "Types in expression must match should.");
            }
            std::shared_ptr<Object> visit(std::shared_ptr<Group> expr) {
                return expr->m_expr->accept(*this);
            }
            std::shared_ptr<Object> visit(std::shared_ptr<Literal> expr) {
                switch(expr->m_token.m_type) {
                    case TokenType::FLOAT:
                        return std::make_shared<Object>(stof(expr->m_token.m_lexeme));
                    case TokenType::INT:
                        return std::make_shared<Object>(stoi(expr->m_token.m_lexeme));
                    case TokenType::STRING:
                        return std::make_shared<Object>(expr->m_token.m_lexeme);
                    case TokenType::TRUE:
                        return std::make_shared<Object>(true);
                    case TokenType::FALSE:
                        return std::make_shared<Object>(false);
                }
            }

            std::shared_ptr<Object> visit(std::shared_ptr<Logic> expr) {
                std::shared_ptr<Object> left = expr->m_left->accept(*this);
                std::shared_ptr<Object> right = expr->m_right->accept(*this);

                switch(expr->m_op.m_type) {
                    case TokenType::OR:
                        return std::make_shared<Object>(left->get_bool() || left->get_bool());
                    case TokenType::AND:
                        return std::make_shared<Object>(left->get_bool() && left->get_bool());
                    case TokenType::EQUAL_EQUAL: //type checker will not allow interpreting if types aren't the same
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() == right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(abs(left->get_float() - right->get_float()) < 0.01f);
                        if(left->is_string())   return std::make_shared<Object>(left->get_string() == right->get_string());
                        if(left->is_bool())     return std::make_shared<Object>(left->get_bool() == right->get_bool());
                        break;
                    case TokenType::BANG_EQUAL:
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() != right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(abs(left->get_float() - right->get_float()) >= 0.01f);
                        if(left->is_string())   return std::make_shared<Object>(left->get_string() != right->get_string());
                        if(left->is_bool())     return std::make_shared<Object>(left->get_bool() != right->get_bool());
                        break;
                    case TokenType::LESS:
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() < right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(left->get_float() < right->get_float());
                        break;
                    case TokenType::LESS_EQUAL:
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() <= right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(
                                                                        left->get_float() < right->get_float() ||
                                                                        abs(left->get_float() - right->get_float()) < 0.01f);
                        break;
                    case TokenType::GREATER:
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() > right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(left->get_float() > right->get_float());
                    case TokenType::GREATER_EQUAL:
                        if(left->is_int())      return std::make_shared<Object>(left->get_int() >= right->get_int());
                        if(left->is_float())    return std::make_shared<Object>(
                                                                        left->get_float() > right->get_float() ||
                                                                        abs(left->get_float() - right->get_float()) < 0.01f);
                        break;
                }

                throw RuntimeError(expr->m_op, "Invalid logical operator.  Equality and inequalities not implemented");

            }


    };

}


#endif //ZEBRA_INTERPRETER_H
