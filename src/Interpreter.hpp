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

            std::shared_ptr<Object> evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            //throw away result of expression
            void visit(ExprStmt* stmt) {
                evaluate(stmt->m_expr.get());
            }

            void visit(Print* stmt) {
                std::shared_ptr<Object> value = stmt->m_value->accept(*this);

                if(dynamic_cast<Bool*>(value.get())) {
                    std::cout << dynamic_cast<Bool*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<Int*>(value.get())) {
                    std::cout << dynamic_cast<Int*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<Float*>(value.get())) {
                    std::cout << dynamic_cast<Float*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<String*>(value.get())) {
                    std::cout << dynamic_cast<String*>(value.get())->m_value << std::endl;
                }
            }

            void visit(If* stmt) {
                std::shared_ptr<Object> condition = evaluate(stmt->m_condition.get());
                if(dynamic_cast<Bool*>(condition.get()) && dynamic_cast<Bool*>(condition.get())->m_value) {
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

            void visit(While* stmt) {
                std::shared_ptr<Expr> condition = stmt->m_condition;
                while(dynamic_cast<Bool*>(evaluate(condition.get()).get())->m_value) {
                    execute(stmt->m_body.get());
                }
            }

            void visit(For* stmt) {
                execute(stmt->m_initializer.get());
                std::shared_ptr<Expr> condition = stmt->m_condition;
                while(dynamic_cast<Bool*>(evaluate(condition.get()).get())->m_value) {
                    execute(stmt->m_body.get());
                    evaluate(stmt->m_update.get()); //not using result of expression
                }
            }

            void visit(Return* stmt) {
                std::shared_ptr<Object> ret = evaluate(stmt->m_value.get());
                //TODO: how to get ret back to the call function???
            }


            std::shared_ptr<Object> visit(Unary* expr) {
                std::shared_ptr<Object> right = expr->m_right->accept(*this);

                if(expr->m_op.m_type == TokenType::MINUS) {
                    if (dynamic_cast<Float*>(right.get())) {
                        return std::make_shared<Float>(-dynamic_cast<Float*>(right.get())->m_value);
                    }else if (dynamic_cast<Int*>(right.get())) {
                        return std::make_shared<Int>(-dynamic_cast<Int*>(right.get())->m_value);
                    }
                }else if(expr->m_op.m_type == TokenType::BANG) {
                    return std::make_shared<Bool>(dynamic_cast<Bool*>(right.get())->m_value);
                }

            }

            std::shared_ptr<Object> visit(Binary* expr) {
                std::shared_ptr<Object> left = expr->m_left->accept(*this);
                std::shared_ptr<Object> right = expr->m_right->accept(*this);

                if(dynamic_cast<Int*>(left.get())) {
                    int a = dynamic_cast<Int*>(left.get())->m_value;
                    int b = dynamic_cast<Int*>(right.get())->m_value;
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Int>(a + b);
                        case TokenType::MINUS: return std::make_shared<Int>(a - b);
                        case TokenType::STAR: return std::make_shared<Int>(a * b);
                        case TokenType::SLASH: return std::make_shared<Int>(a / b);
                        case TokenType::MOD: return std::make_shared<Int>(a % b);
                    }
                }
                if(dynamic_cast<Float*>(left.get())) {
                    float a = dynamic_cast<Float*>(left.get())->m_value;
                    float b = dynamic_cast<Float*>(right.get())->m_value;
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<Float>(a + b);
                        case TokenType::MINUS: return std::make_shared<Float>(a - b);
                        case TokenType::STAR: return std::make_shared<Float>(a * b);
                        case TokenType::SLASH: return std::make_shared<Float>(a / b);
                    }
                }
                if(dynamic_cast<String*>(left.get())) {
                    std::string a = dynamic_cast<String*>(left.get())->m_value;
                    std::string b = dynamic_cast<String*>(right.get())->m_value;
                    switch(expr->m_op.m_type) {
                        case TokenType::PLUS: return std::make_shared<String>(a + b);
                    }
                }

            }
            std::shared_ptr<Object> visit(Group* expr) {
                return expr->m_expr->accept(*this);
            }
            std::shared_ptr<Object> visit(Literal* expr) {
                switch(expr->m_token.m_type) {
                    case TokenType::FLOAT:
                        return std::make_shared<Float>(stof(expr->m_token.m_lexeme));
                    case TokenType::INT:
                        return std::make_shared<Int>(stoi(expr->m_token.m_lexeme));
                    case TokenType::STRING:
                        return std::make_shared<String>(expr->m_token.m_lexeme);
                    case TokenType::TRUE:
                        return std::make_shared<Bool>(true);
                    case TokenType::FALSE:
                        return std::make_shared<Bool>(false);
                }
            }

            std::shared_ptr<Object> visit(Logic* expr) {

                std::shared_ptr<Object> left = expr->m_left->accept(*this);
                std::shared_ptr<Object> right = expr->m_right->accept(*this);

                Bool* bool_left = dynamic_cast<Bool*>(left.get());
                Bool* bool_right = dynamic_cast<Bool*>(right.get());
                if(bool_left && bool_right) {
                    switch(expr->m_op.m_type) {
                        case TokenType::OR:
                            return std::make_shared<Bool>(bool_left->m_value || bool_right->m_value);
                        case TokenType::AND:
                            return std::make_shared<Bool>(bool_left->m_value && bool_right->m_value);
                        case TokenType::EQUAL_EQUAL: 
                            return std::make_shared<Bool>(bool_left->m_value == bool_right->m_value);
                        case TokenType::BANG_EQUAL:
                            return std::make_shared<Bool>(bool_left->m_value != bool_right->m_value);
                    }
                }

                Int* int_left = dynamic_cast<Int*>(left.get());
                Int* int_right = dynamic_cast<Int*>(right.get());
                if(int_left && int_right) {
                    switch(expr->m_op.m_type) {
                        case TokenType::EQUAL_EQUAL:
                            return std::make_shared<Bool>(int_left->m_value == int_right->m_value);
                        case TokenType::BANG_EQUAL:
                            return std::make_shared<Bool>(int_left->m_value != int_right->m_value);
                        case TokenType::LESS:
                            return std::make_shared<Bool>(int_left->m_value < int_right->m_value);
                        case TokenType::LESS_EQUAL:
                            return std::make_shared<Bool>(int_left->m_value <= int_right->m_value);
                        case TokenType::GREATER:
                            return std::make_shared<Bool>(int_left->m_value > int_right->m_value);
                        case TokenType::GREATER_EQUAL:
                            return std::make_shared<Bool>(int_left->m_value >= int_right->m_value);
                    }
                }

                Float* float_left   =   dynamic_cast<Float*>(left.get());
                Float* float_right   =   dynamic_cast<Float*>(right.get());
                if(float_left && float_right) {
                    switch(expr->m_op.m_type) {
                        case TokenType::EQUAL_EQUAL: 
                            return std::make_shared<Bool>(abs(float_left->m_value - float_right->m_value) < 0.01f);
                        case TokenType::BANG_EQUAL:
                            return std::make_shared<Bool>(abs(float_left->m_value - float_right->m_value) >= 0.01f);
                        case TokenType::LESS:
                            return std::make_shared<Bool>(float_left->m_value < float_right->m_value);
                        case TokenType::LESS_EQUAL:
                            return std::make_shared<Bool>(float_left->m_value < float_right->m_value ||
                                                            abs(float_left->m_value - float_right->m_value) < 0.01f);
                        case TokenType::GREATER:
                            return std::make_shared<Bool>(float_left->m_value > float_right->m_value);
                        case TokenType::GREATER_EQUAL:
                            return std::make_shared<Bool>(float_left->m_value > float_right->m_value ||
                                                            abs(float_left->m_value - float_right->m_value) < 0.01f);
                    }
                }

                String* string_left =   dynamic_cast<String*>(left.get());
                String* string_right =   dynamic_cast<String*>(right.get());
                if(string_left && string_right) {
                    switch(expr->m_op.m_type) {
                        case TokenType::EQUAL_EQUAL: 
                            return std::make_shared<Bool>(string_left->m_value == string_right->m_value);
                        case TokenType::BANG_EQUAL:
                            return std::make_shared<Bool>(string_left->m_value != string_right->m_value);
                    }
                }

            }



            std::shared_ptr<Object> visit(Assign* expr) {
                std::shared_ptr<Object> value = evaluate(expr->m_value.get());
                m_environment->assign(expr->m_name, value);
                return value;
            }

            std::shared_ptr<Object> visit(VarDecl* expr) {
                std::shared_ptr<Object> value = evaluate(expr->m_value.get());
                m_environment->define(expr->m_name, value);
                return value;
            }

            std::shared_ptr<Object> visit(FunDecl* expr) {
                std::shared_ptr<Object> fun = std::make_shared<Fun>(expr);
                m_environment->define(expr->m_name, fun);
                return fun;
            }

            std::shared_ptr<Object> visit(Call* expr) {
                std::shared_ptr<Object> obj = m_environment->get(expr->m_name);
                Fun* fun = dynamic_cast<Fun*>(obj.get());
                FunDecl* fun_decl = dynamic_cast<FunDecl*>(fun->m_fun_decl);

                //evaluate call arguments
                std::vector<std::shared_ptr<Object>> arguments;
                for (std::shared_ptr<Expr> e: expr->m_arguments) {
                    arguments.push_back(evaluate(e.get()));
                }
                
                std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment);
                std::shared_ptr<Environment> closure = m_environment;
                m_environment = block_env;

                //declare and define parameters in FunDecl
                //type checker should have already verified that parameter/argument types/counts match
                for (int i = 0; i < fun_decl->m_parameters.size(); i++) {
                    Token param_token = dynamic_cast<VarDecl*>(fun_decl->m_parameters.at(i).get())->m_name;
                    std::shared_ptr<Object> param_value = arguments.at(i);
                    m_environment->define(param_token, param_value);
                }

                execute(fun_decl->m_body.get()); //return needs to happen here

                m_environment = closure;   

                return nullptr; //TODO: need to integrate return values
            } 

            std::shared_ptr<Object> visit(Variable* expr) {
                return m_environment->get(expr->m_name);
            }


    };

}


#endif //ZEBRA_INTERPRETER_H
