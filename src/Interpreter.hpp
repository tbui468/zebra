#ifndef ZEBRA_INTERPRETER_H
#define ZEBRA_INTERPRETER_H

#include <vector>
#include <iostream>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Object.hpp"
#include "Environment.hpp"

namespace zebra {

    class Interpreter: public StmtVoidVisitor, public ExprObjectVisitor {
        private:
            std::vector<std::shared_ptr<Stmt>> m_statements;
            std::shared_ptr<Environment> m_environment;
        public:
            Interpreter(const std::vector<std::shared_ptr<Stmt>> statements): m_statements(statements) {
                m_environment = std::make_shared<Environment>();
            }
            ~Interpreter() {}
            void run() {
                for(std::shared_ptr<Stmt> s: m_statements) {
                    execute(s.get());
                }
            }
            void execute(Stmt* stmt) {
                stmt->accept(*this);
            }

            void visit(Print* stmt) {
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

            void visit(If* stmt) {
                std::shared_ptr<Object> condition = evaluate(stmt->m_condition.get());
                if(condition->is_truthy()) {
                    execute(stmt->m_then_branch.get());                    
                }else if(stmt->m_else_branch) {
                    execute(stmt->m_else_branch.get());
                }
            }

            void visit(Block* stmt) {
                std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment);
                std::shared_ptr<Environment> closure = m_environment;
                m_environment = block_env;
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    execute(s.get());  
                } 
                m_environment = closure;   
            }

            //note: stmt->m_type is used in type checker only    
            void visit(VarDecl* stmt) {
                std::shared_ptr<Object> value = evaluate(stmt->m_value.get());
                m_environment->define(stmt->m_name, value);
            }


            void visit(AssignStmt* stmt) {
                std::shared_ptr<Object> value = evaluate(stmt->m_value.get());
                m_environment->assign(stmt->m_name, value);

            }

            void visit(While* stmt) {
                while(evaluate(stmt->m_condition.get())->get_bool()) {
                    execute(stmt->m_body.get());
                }
            }

            void visit(For* stmt) {
                execute(stmt->m_initializer.get());
                while(evaluate(stmt->m_condition.get())->get_bool()) {
                    execute(stmt->m_body.get());
                    std::shared_ptr<Object> up = evaluate(stmt->m_update.get()); //not using result of expression
                }
            }

            std::shared_ptr<Object> evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            std::shared_ptr<Object> visit(Unary* expr) {
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

            }

            std::shared_ptr<Object> visit(Binary* expr) {
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
                }
                if(left->is_string() && right->is_string()) {
                    std::string a = left->get_string();
                    std::string b = right->get_string();
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Object>(a + b);
                    }
                }

            }
            std::shared_ptr<Object> visit(Group* expr) {
                return expr->m_expr->accept(*this);
            }
            std::shared_ptr<Object> visit(Literal* expr) {
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

            std::shared_ptr<Object> visit(Logic* expr) {
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


            }


            std::shared_ptr<Object> visit(AssignExpr* expr) {
                std::shared_ptr<Object> value = evaluate(expr->m_value.get());
                m_environment->assign(expr->m_name, value);
                return value;
            }

            std::shared_ptr<Object> visit(Variable* expr) {
                return m_environment->get(expr->m_name);
            }


    };

}


#endif //ZEBRA_INTERPRETER_H
