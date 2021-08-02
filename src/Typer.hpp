#ifndef ZEBRA_TYPE_CHECKER_H
#define ZEBRA_TYPE_CHECKER_H

#include <vector>
#include <unordered_map>
#include <iostream>

#include "TokenType.hpp"
#include "Token.hpp"
#include "Expr.hpp"
#include "ResultCode.hpp"

namespace zebra {


    class TypeError {
        private:
            Token m_token;
            std::string m_message;
        public:
            TypeError(Token token, const std::string& message): m_token(token), m_message(message){}
            ~TypeError() {}
            void print() {
                std::cout << "[Line " << m_token.m_line << "] Type Error: " << m_message << std::endl;
            }
    };


    class Typer: public ExprTokenTypeVisitor {
        private:
            //TODO: m_variables is a stack of environments - how do we deal with the
            //local environments of class instances?
            //std::vector<std::unordered_map<std::string, Expr*>> m_variables;
            std::vector<std::unordered_map<std::string, TokenType>> m_types;
            std::vector<TypeError> m_errors;
        public:
            Typer() {
                m_types.emplace_back(std::unordered_map<std::string, TokenType>()); 
            }

            ~Typer() {}

            ResultCode type(const std::vector<std::shared_ptr<Expr>>& ast) {
                for(std::shared_ptr<Expr> expr: ast) {
                    evaluate(expr.get());
                }

                if (m_errors.empty()) {
                    return ResultCode::SUCCESS;
                } else {
                    return ResultCode::FAILED;
                }
            }
            std::vector<TypeError> get_errors() {
                return m_errors;
            }
        private:
            TokenType evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            /*
             * Helper Functions
             */

            void add_error(Token token, const std::string& message) {
                m_errors.push_back(TypeError(token, message));
            }

            /*
            TokenType get_field_type_from_instance(Token inst_name, Token field) {
                StructInst* inst = dynamic_cast<StructInst*>(get_decl(inst_name));
                StructDecl* decl = dynamic_cast<StructDecl*>(get_decl(inst->m_struct)); 

                for (std::shared_ptr<VarDecl> var_decl: decl->m_fields) {
                    if (var_decl->m_name.m_lexeme == field.m_lexeme) {
                        return var_decl->m_type.m_type;
                    }
                }

                throw TypeError(inst_name, "Struct " + decl->m_name.to_string() + " has does not have a field " + field.to_string());
            }

            Stmt* get_decl(Token& token) {
                for (int i = m_variables.size() - 1; i >= 0; i--) {
                    if(m_variables.at(i).count(token.m_lexeme) > 0) {
                        return m_variables.at(i)[token.m_lexeme];
                    }
                }

                throw TypeError(token, token.to_string() + " not declared.");
            }

            //getting declaration type (variable or function return type)
            TokenType get_type(Token& token) {
                Stmt* var = get_decl(token);

                if (dynamic_cast<VarDecl*>(var)) {
                    return dynamic_cast<VarDecl*>(var)->m_type.m_type;
                } else if (dynamic_cast<StructInst*>(var)) {
                    return TokenType::STRUCT_TYPE;                     
                } else {
                    return dynamic_cast<FunDecl*>(var)->m_return_type;
                }
            }*/


            /*
             * Misc.
             */
            TokenType visit(Import* expr) {
                return TokenType::NIL_TYPE;
            }


            /*
             * Basic
             */
            TokenType visit(Unary* expr) {
                TokenType right_type = evaluate(expr->m_right.get());

                switch(expr->m_op.m_type) {
                    case TokenType::MINUS:
                        if(right_type == TokenType::INT_TYPE || right_type == TokenType::FLOAT_TYPE)
                            return right_type;
                    case TokenType::BANG:
                        if(right_type == TokenType::BOOL_TYPE)
                            return right_type;
                }

                add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + 
                                      " operator on a" + Token::to_string(right_type) + ".");
                return TokenType::ERROR;
            }

            TokenType visit(Binary* expr) {
                TokenType left = evaluate(expr->m_left.get());
                TokenType right = evaluate(expr->m_right.get());

                if (left == right && left != TokenType::BOOL_TYPE) return left;

                add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + 
                                      " operator with a " + Token::to_string(left) + 
                                      " and a " + Token::to_string(right) + ".");
                return TokenType::ERROR;
            }

            TokenType visit(Group* expr) {
                return evaluate(expr->m_expr.get());
            }

            TokenType visit(Literal* expr) {
                switch(expr->m_token.m_type) {
                    case TokenType::TRUE:
                    case TokenType::FALSE:
                        return TokenType::BOOL_TYPE;
                    case TokenType::FLOAT:
                        return TokenType::FLOAT_TYPE;
                    case TokenType::INT:
                        return TokenType::INT_TYPE;
                    case TokenType::STRING:
                        return TokenType::STRING_TYPE;
                }

                add_error(expr->m_token, expr->m_token.to_string() + " token not valid.");
                return TokenType::ERROR;
            }

            // ==, !=, <, <=, >, >=, and, or
            TokenType visit(Logic* expr) {
                TokenType left = evaluate(expr->m_left.get());
                TokenType right = evaluate(expr->m_right.get());

                if (left != right) {
                    add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + 
                                          " with " + Token::to_string(left) + 
                                          " and " + Token::to_string(right) + ".");
                    return TokenType::ERROR;
                }

                bool logic = expr->m_op.m_type == TokenType::AND ||
                                   expr->m_op.m_type == TokenType::OR;

                if (logic && left != TokenType::BOOL_TYPE) {
                    add_error(expr->m_op, "The expressions on the left and right of " + 
                                          expr->m_op.to_string() + " must evaluate to boolean types.");
                    return TokenType::ERROR;
                }

                bool inequality = expr->m_op.m_type == TokenType::LESS ||
                                   expr->m_op.m_type == TokenType::LESS_EQUAL ||
                                   expr->m_op.m_type == TokenType::GREATER ||
                                   expr->m_op.m_type == TokenType::GREATER_EQUAL;

                if ((left == TokenType::BOOL_TYPE || right == TokenType::BOOL_TYPE) && inequality) {
                    add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + " with boolean types.");
                    return TokenType::ERROR;
                }

                return TokenType::BOOL_TYPE;
            }

            /*
             * Variables and Functions
             */

            TokenType visit(DeclVar* expr) {
                if (expr->m_value) {
                    if (evaluate(expr->m_value.get()) != expr->m_type.m_type) {
                        add_error(expr->m_name, "Right hand side of " + expr->m_name.to_string() + 
                                                " must evaluate to " + expr->m_type.to_string() + ".");
                        return TokenType::ERROR;
                    }
                } else {
                    add_error(expr->m_name, expr->m_name.to_string() + " must be defined at declaration.");
                    return TokenType::ERROR;
                }

                m_types.back()[expr->m_name.m_lexeme] = expr->m_type.m_type;

                return expr->m_type.m_type;
            }

            TokenType visit(GetVar* expr) {
                if (m_types.back().count(expr->m_name.m_lexeme) == 0) {
                    add_error(expr->m_name, "Undefined reference to " + expr->m_name.to_string() + ".");
                    return TokenType::ERROR;
                } else {
                    return m_types.back()[expr->m_name.m_lexeme];
                }
            }

            TokenType visit(SetVar* expr) {
                if (m_types.back().count(expr->m_name.m_lexeme) == 0) {
                    add_error(expr->m_name, "Undefined reference to " + expr->m_name.to_string() + ".");
                    return TokenType::ERROR;
                } else {
                    TokenType var_type = m_types.back()[expr->m_name.m_lexeme];
                    TokenType val_type = evaluate(expr->m_value.get());
                    if (var_type != val_type) {
                        add_error(expr->m_name, "Cannot assign variable of " + Token::to_string(var_type) + 
                                                " to expression evaluating to " + Token::to_string(val_type) + ".");
                        return TokenType::ERROR;
                    }

                    return var_type;
                }
            }

            TokenType visit(DeclFun* stmt) {
                return TokenType::NIL;
                /*
                m_variables.back()[stmt->m_name.m_lexeme] = stmt;
                for(std::shared_ptr<Stmt> s: stmt->m_parameters) {
                    VarDecl* var_decl = dynamic_cast<VarDecl*>(s.get());
                    m_variables.back()[var_decl->m_name.m_lexeme] = s.get();
                }

                execute(stmt->m_body.get());*/
            }

            TokenType visit(CallFun* stmt) {
                return TokenType::NIL;
                /*
                FunDecl* fun_decl = dynamic_cast<FunDecl*>(get_decl(stmt->m_name));

                //check parameter/argument count
                if(stmt->m_arity != fun_decl->m_arity) {
                    throw TypeError(stmt->m_name, "Function call argument count must match declaration parameter count.");
                }

                //check parameter/argument type
                for (int i = 0; i < stmt->m_arguments.size(); i++) {
                    TokenType arg_type = evaluate(stmt->m_arguments.at(i).get());
                    VarDecl* param = dynamic_cast<VarDecl*>(fun_decl->m_parameters.at(i).get());
                    if(arg_type != get_type(param->m_name)) {
                        throw TypeError(stmt->m_name, "Function call argument type must match declaration parameter type.");
                    }
                }*/
            }

            TokenType visit(Return* stmt) {
                return TokenType::NIL;
                /*
                TokenType expr_type = evaluate(stmt->m_value.get());
                if(expr_type != stmt->m_return_type) {
                    throw TypeError(stmt->m_name, "Return type must match return type in function declaration.");
                }*/
            }


            /*
             * Control Flow
             */

            TokenType visit(Block* expr) {
                m_types.emplace_back(std::unordered_map<std::string, TokenType>());
                for(std::shared_ptr<Expr> e: expr->m_expressions) {
                    evaluate(e.get());
                }
                m_types.pop_back();
                return TokenType::NIL_TYPE;
            }

            TokenType visit(If* expr) {
                TokenType condition = evaluate(expr->m_condition.get());
                if (condition != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "If condition cannot evaluate to a " + Token::to_string(condition) + ".");
                    return TokenType::ERROR;
                }
                evaluate(expr->m_then_branch.get());
                if(expr->m_else_branch) evaluate(expr->m_else_branch.get());
                return TokenType::NIL_TYPE;
            }

            TokenType visit(For* expr) {
                evaluate(expr->m_initializer.get());
                TokenType condition = evaluate(expr->m_condition.get());
                evaluate(expr->m_update.get());
                if (condition != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "For loop condition cannot evaluate to a " + Token::to_string(condition) + ".");
                    return TokenType::ERROR;
                }
                evaluate(expr->m_body.get());
                return TokenType::NIL_TYPE;
            }

            TokenType visit(While* expr) {
                TokenType condition = evaluate(expr->m_condition.get());
                if (condition != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "For loop condition cannot evaluate to a " + Token::to_string(condition) + ".");
                    return TokenType::ERROR;
                }
                evaluate(expr->m_body.get());
                return TokenType::NIL_TYPE;
            }

            /*
             * Classes
             */
            TokenType visit(DeclClass* stmt) {
                return TokenType::NIL;
                /*
                m_variables.back()[stmt->m_name.m_lexeme] = stmt;*/
            }

            TokenType visit(InstClass* stmt) {
                return TokenType::NIL;
                /*
                m_variables.back()[stmt->m_name.m_lexeme] = stmt;
                Stmt* decl = get_decl(stmt->m_struct);
                StructDecl* struct_decl = dynamic_cast<StructDecl*>(decl);

                if (stmt->m_arguments.size() > 0 && stmt->m_arguments.size() != struct_decl->m_fields.size()) {
                    throw TypeError(stmt->m_name, "Number of arguments for " + stmt->m_name.to_string() + " must be 0 or match the declaration.");
                }

                for (int i = 0; i < stmt->m_arguments.size(); i++) {
                    TokenType arg_type = evaluate(stmt->m_arguments.at(i).get());
                    TokenType field_type = struct_decl->m_fields.at(i)->m_type.m_type;
                    if (arg_type != field_type) {
                        throw TypeError(stmt->m_name, "Arguments for " + stmt->m_name.to_string() + " must match declaration.");
                    }
                }*/
            }

            TokenType visit(GetField* expr) {
                return TokenType::NIL;
                /*
                return get_field_type_from_instance(expr->m_instance, expr->m_field);*/
            }

            TokenType visit(SetField* expr) {
                return TokenType::NIL;
                /*
                TokenType field_type = get_field_type_from_instance(stmt->m_instance, stmt->m_field);
                TokenType value_t = evaluate(stmt->m_value.get());

                if (field_type != value_t) {
                    throw TypeError(stmt->m_instance, stmt->m_field.to_string() + " type does not match assignment type.");
                }*/
            }

            TokenType visit(CallMethod* expr) {
                return TokenType::NIL;
            }


    };
}


#endif //ZEBRA_TYPE_CHECKER_H
