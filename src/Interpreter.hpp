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
                if (m_environment->get_return()) {
                    return;
                }
                stmt->accept(*this);
            }

            std::shared_ptr<Object> evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            void visit(Print* stmt) {
                std::shared_ptr<Object> value = evaluate(stmt->m_value.get());

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
                std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, false);
                std::shared_ptr<Environment> closure = m_environment;
                m_environment = block_env;
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    execute(s.get());  
                    if( dynamic_cast<Return*>(s.get())) {
                        break;
                    }
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
                m_environment->set_return(ret);
            }

            void visit(Assign* stmt) {
                std::shared_ptr<Object> value = evaluate(stmt->m_value.get());
                m_environment->assign(stmt->m_name, value);
            }

            void visit(VarDecl* stmt) {
                std::shared_ptr<Object> value = evaluate(stmt->m_value.get());
                m_environment->define(stmt->m_name, value);
            }

            void visit(FunDecl* stmt) {
                std::shared_ptr<Object> fun = std::make_shared<Fun>(stmt);
                m_environment->define(stmt->m_name, fun);
            }

            void visit(StructDecl* stmt) {
                std::shared_ptr<Object> struct_def = std::make_shared<StructDefinition>(stmt);
                m_environment->define(stmt->m_name, struct_def);
            }

            void visit(StructInst* stmt) {
                StructDefinition* def = dynamic_cast<StructDefinition*>(m_environment->get(stmt->m_struct).get());
                std::unordered_map<std::string, std::shared_ptr<Object>> fields;
                if (!(stmt->m_arguments.empty())) {
                    for (int i = 0; i < def->m_node->m_fields.size(); i++) {
                        //get lexeme from declaration
                        std::string lexeme = def->m_node->m_fields.at(i)->m_name.m_lexeme;
                        //get value from argument evaluation
                        std::shared_ptr<Object> value = evaluate(stmt->m_arguments.at(i).get());
                        fields[lexeme] = value;
                    }
                } else { //use default values in struct definition
                    for (std::shared_ptr<VarDecl> var_decl: def->m_node->m_fields) {
                        std::string lexeme = var_decl->m_name.m_lexeme;
                        std::shared_ptr<Object> value = evaluate(var_decl->m_value.get());
                        fields[lexeme] = value;
                    }
                }

                std::shared_ptr<Object> struct_instance = std::make_shared<StructInstance>(fields);
                m_environment->define(stmt->m_name, struct_instance);
            }

            void visit(Call* stmt) {
                std::shared_ptr<Object> obj = m_environment->get(stmt->m_name);
                Fun* fun = dynamic_cast<Fun*>(obj.get());
                FunDecl* fun_decl = dynamic_cast<FunDecl*>(fun->m_fun_decl);

                //evaluate call arguments
                std::vector<std::shared_ptr<Object>> arguments;
                for (std::shared_ptr<Expr> e: stmt->m_arguments) {
                    arguments.push_back(evaluate(e.get()));
                }
                
                std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, true);
                std::shared_ptr<Environment> closure = m_environment;
                m_environment = block_env;

                //declare and define parameters in FunDecl
                //type checker should have already verified that parameter/argument types/counts match
                for (int i = 0; i < fun_decl->m_parameters.size(); i++) {
                    Token param_token = dynamic_cast<VarDecl*>(fun_decl->m_parameters.at(i).get())->m_name;
                    std::shared_ptr<Object> param_value = arguments.at(i);
                    m_environment->define(param_token, param_value);
                }

                execute(fun_decl->m_body.get()); //fun_decl->m_body is a Block, which will already create its own environment

                std::shared_ptr<Object> return_value = m_environment->get_return();

                m_environment = closure;   

                stmt->m_return = return_value;
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
                    case TokenType::NIL:
                        return std::make_shared<Nil>();
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

            std::shared_ptr<Object> visit(Variable* expr) {
                return m_environment->get(expr->m_name);
            }

            std::shared_ptr<Object> visit(Access* expr) {
                StructInstance* inst = dynamic_cast<StructInstance*>(m_environment->get(expr->m_instance).get());
                return inst->m_fields[expr->m_field.m_lexeme];
            }

            std::shared_ptr<Object> visit(StmtExpr* expr) {
                execute(expr->m_stmt.get());
                
                Call* call = dynamic_cast<Call*>(expr->m_stmt.get());
                if (call) {
                    return call->m_return;
                }

                Assign* assignment = dynamic_cast<Assign*>(expr->m_stmt.get());
                if (assignment) {
                    return m_environment->get(assignment->m_name);
                }

                VarDecl* var_decl = dynamic_cast<VarDecl*>(expr->m_stmt.get());
                if (var_decl) {
                    return m_environment->get(var_decl->m_name);
                }
            }



    };

}


#endif //ZEBRA_INTERPRETER_H
