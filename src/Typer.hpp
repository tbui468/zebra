#ifndef ZEBRA_TYPE_CHECKER_H
#define ZEBRA_TYPE_CHECKER_H

#include <vector>
#include <unordered_map>
#include <iostream>

#include "TokenType.hpp"
#include "Token.hpp"
#include "Expr.hpp"
#include "ResultCode.hpp"
#include "DataType.hpp"

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


    class Typer: public DataTypeVisitor {
        private:
            struct ClassSig {
                std::unordered_map<std::string, DataType> m_field_sig;
                std::unordered_map<std::string, std::vector<DataType>> m_method_sig;
            };
            std::vector<std::unordered_map<std::string, ClassSig>> m_class_sig;
            std::vector<std::unordered_map<std::string, DataType>> m_var_sig;
            std::vector<std::unordered_map<std::string, std::vector<DataType>>> m_fun_sig;
            std::vector<TypeError> m_errors;
        public:
            Typer() {
                push_scope();
            }

            ~Typer() {}

            void push_scope() {
                m_var_sig.emplace_back(std::unordered_map<std::string, DataType>()); 
                m_fun_sig.emplace_back(std::unordered_map<std::string, std::vector<DataType>>()); 
                m_class_sig.emplace_back(std::unordered_map<std::string, ClassSig>());
            }

            void pop_scope() {
                m_var_sig.pop_back();
                m_fun_sig.pop_back();
                m_class_sig.pop_back();
            }

            DataType find_var_sig(const std::string& lexeme) {
                for (int i = m_var_sig.size() - 1; i >= 0; i--) {
                    if (m_var_sig.at(i).count(lexeme) > 0) 
                        return m_var_sig.at(i)[lexeme];
                    else
                        continue;
                }
            }

            std::vector<DataType> find_fun_sig(const std::string& lexeme) {
                for (int i = m_fun_sig.size() - 1; i >= 0; i--) {
                    if (m_fun_sig.at(i).count(lexeme) > 0) 
                        return m_fun_sig.at(i)[lexeme];
                    else
                        continue;
                }
            }

            ClassSig find_class_sig(const std::string& lexeme) {
                for (int i = m_class_sig.size() - 1; i >= 0; i--) {
                    if (m_class_sig.at(i).count(lexeme) > 0) 
                        return m_class_sig.at(i)[lexeme];
                    else
                        continue;
                } 
            }

            bool is_declared_var(const std::string& lexeme) {
                for (int i = m_var_sig.size() - 1; i >= 0; i--) {
                    if (m_var_sig.at(i).count(lexeme) > 0) 
                        return true;
                    else
                        continue;
                }
                return false;
            }

            bool is_declared_fun(const std::string& lexeme) {
                for (int i = m_fun_sig.size() - 1; i >= 0; i--) {
                    if (m_fun_sig.at(i).count(lexeme) > 0) 
                        return true;
                    else
                        continue;
                }

                return false;
            }

            bool is_declared_class(const std::string& lexeme) {
                for (int i = m_class_sig.size() - 1; i >= 0; i--) {
                    if (m_class_sig.at(i).count(lexeme) > 0) 
                        return true;
                    else
                        continue;
                }

                return false;
            }

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
            DataType evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            void add_error(Token token, const std::string& message) {
                m_errors.push_back(TypeError(token, message));
            }


            /*
             * Misc.
             */
            DataType visit(Import* expr) {
                for (std::pair<std::string, std::shared_ptr<Object>> p: expr->m_functions) {
                    m_fun_sig.back()[p.first] = dynamic_cast<Callable*>(p.second.get())->m_signature;
                }
                return DataType(TokenType::NIL_TYPE);
            }


            /*
             * Basic
             */
            DataType visit(Unary* expr) {
                DataType right_type = evaluate(expr->m_right.get());

                if (right_type.m_type == TokenType::CUSTOM_TYPE) {
                    add_error(expr->m_op, expr->m_op.to_string() + " operator does not work on " +
                                          right_type.m_lexeme + " data types.");
                    return DataType(TokenType::ERROR);
                }

                switch(expr->m_op.m_type) {
                    case TokenType::MINUS:
                        if(right_type.m_type == TokenType::INT_TYPE || right_type.m_type == TokenType::FLOAT_TYPE)
                            return right_type;
                    case TokenType::BANG:
                        if(right_type.m_type == TokenType::BOOL_TYPE)
                            return right_type;
                }

                add_error(expr->m_op, "Cannot use " + 
                                      expr->m_op.to_string() + 
                                      " operator on a" + 
                                      Token::to_string(right_type.m_type) + ".");

                return DataType(TokenType::ERROR);
            }

            DataType visit(Binary* expr) {
                DataType left = evaluate(expr->m_left.get());
                DataType right = evaluate(expr->m_right.get());

                if (left.m_type == TokenType::CUSTOM_TYPE ||
                    right.m_type == TokenType::CUSTOM_TYPE) {
                    add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() +
                                          " operator with a " + left.m_lexeme + 
                                          " and a " + right.m_lexeme + ".");
                    return DataType(TokenType::ERROR);
                }

                if (left.m_type == right.m_type && 
                    left.m_type != TokenType::BOOL_TYPE &&
                    left.m_type != TokenType::CUSTOM_TYPE) return left;

                add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + 
                                      " operator with a " + Token::to_string(left.m_type) + 
                                      " and a " + Token::to_string(right.m_type) + ".");
                return DataType(TokenType::ERROR);
            }

            DataType visit(Group* expr) {
                return evaluate(expr->m_expr.get());
            }

            DataType visit(Literal* expr) {
                switch(expr->m_token.m_type) {
                    case TokenType::TRUE:
                    case TokenType::FALSE:
                        return DataType(TokenType::BOOL_TYPE);
                    case TokenType::FLOAT:
                        return DataType(TokenType::FLOAT_TYPE);
                    case TokenType::INT:
                        return DataType(TokenType::INT_TYPE);
                    case TokenType::STRING:
                        return DataType(TokenType::STRING_TYPE);
                }

                add_error(expr->m_token, expr->m_token.to_string() + 
                                         " token not valid.");
                return DataType(TokenType::ERROR);
            }

            // ==, !=, <, <=, >, >=, and, or
            DataType visit(Logic* expr) {
                DataType left = evaluate(expr->m_left.get());
                DataType right = evaluate(expr->m_right.get());

                if (left.m_type == TokenType::CUSTOM_TYPE ||
                    right.m_type == TokenType::CUSTOM_TYPE) {
                    add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() +
                                          " operator with a " + left.m_lexeme + 
                                          " and a " + right.m_lexeme + ".");
                    return DataType(TokenType::ERROR);
                }

                if (left.m_type != right.m_type) {
                    add_error(expr->m_op, "Cannot use " + expr->m_op.to_string() + 
                                          " with " + Token::to_string(left.m_type) + 
                                          " and " + Token::to_string(right.m_type) + ".");
                    return DataType(TokenType::ERROR);
                }

                bool logic = expr->m_op.m_type == TokenType::AND ||
                                   expr->m_op.m_type == TokenType::OR;

                if (logic && left.m_type != TokenType::BOOL_TYPE) {
                    add_error(expr->m_op, "The expressions on the left and right of " + 
                                          expr->m_op.to_string() + 
                                          " must evaluate to boolean types.");
                    return DataType(TokenType::ERROR);
                }

                bool inequality = expr->m_op.m_type == TokenType::LESS ||
                                   expr->m_op.m_type == TokenType::LESS_EQUAL ||
                                   expr->m_op.m_type == TokenType::GREATER ||
                                   expr->m_op.m_type == TokenType::GREATER_EQUAL;

                if ((left.m_type == TokenType::BOOL_TYPE || right.m_type == TokenType::BOOL_TYPE) && inequality) {
                    add_error(expr->m_op, "Cannot use " + 
                                          expr->m_op.to_string() + 
                                          " with boolean types.");
                    return DataType(TokenType::ERROR);
                }

                return DataType(TokenType::BOOL_TYPE);
            }

            /*
             * Variables and Functions
             */

            DataType visit(DeclVar* expr) {
                if (expr->m_value) {
                    if (evaluate(expr->m_value.get()).m_type != expr->m_type.m_type) {
                        add_error(expr->m_name, "Right hand side of " + 
                                                expr->m_name.to_string() + 
                                                " must evaluate to " + 
                                                expr->m_type.to_string() + ".");
                        return DataType(TokenType::ERROR);
                    }
                } else {
                    add_error(expr->m_name, expr->m_name.to_string() + 
                                            " must be defined at declaration.");
                    return DataType(TokenType::ERROR);
                }

                m_var_sig.back()[expr->m_name.m_lexeme] = DataType(expr->m_type.m_type);

                return DataType(expr->m_type.m_type);
            }

            DataType visit(GetVar* expr) {
                if (m_var_sig.back().count(expr->m_name.m_lexeme) == 0) {
                    add_error(expr->m_name, "Undefined reference to " + 
                                            expr->m_name.to_string() + ".");
                    return DataType(TokenType::ERROR);
                } else {
                    return find_var_sig(expr->m_name.m_lexeme);
                }
            }

            DataType visit(SetVar* expr) {
                if (m_var_sig.back().count(expr->m_name.m_lexeme) == 0) {
                    add_error(expr->m_name, "Undefined reference to " + 
                                            expr->m_name.to_string() + ".");
                    return DataType(TokenType::ERROR);
                } else {
                    DataType var_type = find_var_sig(expr->m_name.m_lexeme);
                    DataType val_type = evaluate(expr->m_value.get());
                    if (!DataType::equal(var_type, val_type)) {
                        add_error(expr->m_name, "Cannot assign variable of " + 
                                                Token::to_string(var_type.m_type) + 
                                                " to expression evaluating to " + 
                                                Token::to_string(val_type.m_type) + ".");
                        return DataType(TokenType::ERROR);
                    }

                    return var_type;
                }
            }

            DataType visit(DeclFun* expr) {
                std::vector<DataType> types;
                for(std::shared_ptr<Expr> e: expr->m_parameters) {
                    DeclVar* decl_var = dynamic_cast<DeclVar*>(e.get());
                    types.push_back(DataType(decl_var->m_type.m_type));
                }
                types.push_back(DataType(expr->m_return_type));

                m_fun_sig.back()[expr->m_name.m_lexeme] = types;

                push_scope();

                //declaring parameters in local function scope
                for(std::shared_ptr<Expr> e: expr->m_parameters) {
                    DeclVar* decl_var = dynamic_cast<DeclVar*>(e.get());
                    m_var_sig.back()[decl_var->m_name.m_lexeme] = DataType(decl_var->m_type.m_type);
                }

                //checking body of function (may be multiple return statements - need to check them all)
                std::vector<DataType> returns;
                Block* block = dynamic_cast<Block*>(expr->m_body.get());
                for (std::shared_ptr<Expr> e: block->m_expressions) {
                    if (dynamic_cast<Return*>(e.get())) {
                        returns.push_back(evaluate(e.get()));
                    } else {
                        evaluate(e.get());
                    }
                }

                pop_scope();

                if (returns.empty()) {
                    return DataType(TokenType::NIL_TYPE);
                }

                for (DataType ret: returns) {
                    if (ret.m_type != expr->m_return_type) {
                        add_error(expr->m_name, "Return type does not match " + 
                                                expr->m_name.to_string() +
                                                " return type, " + 
                                                Token::to_string(expr->m_return_type) + ".");
                        return DataType(TokenType::ERROR);
                    }
                }

                return DataType(expr->m_return_type);

            }

            DataType visit(CallFun* expr) {
                std::vector<DataType> sig = find_fun_sig(expr->m_name.m_lexeme);

                //function signature includes return type, so it's one size larger than arity
                if (sig.size() - 1 != expr->m_arity) {
                    add_error(expr->m_name, "Number of arguments do no match " + 
                                            expr->m_name.to_string() + " declaration.");
                    return DataType(TokenType::ERROR);
                }

                //check argument types
                for (int i = 0; i < expr->m_arguments.size(); i++) {
                    DataType arg_type = evaluate(expr->m_arguments.at(i).get());
                    DataType sig_type = sig.at(i);
                    
                    if (!DataType::equal(arg_type, sig_type)) {
                        add_error(expr->m_name, "Argument type at position " + 
                                                std::to_string(i) + ", " + 
                                                Token::to_string(arg_type.m_type) + 
                                               ", does not match parameter type " +
                                               Token::to_string(sig_type.m_type));
                        return DataType(TokenType::ERROR);
                    }
                }

                return sig.at(sig.size() - 1);
            }

            DataType visit(Return* expr) {
                if (expr->m_value) {
                    return evaluate(expr->m_value.get());
                } else {
                    return DataType(TokenType::NIL_TYPE);
                }
            }


            /*
             * Control Flow
             */

            DataType visit(Block* expr) {
                push_scope();
                for(std::shared_ptr<Expr> e: expr->m_expressions) {
                    evaluate(e.get());
                }
                pop_scope();

                return DataType(TokenType::NIL_TYPE);
            }

            DataType visit(If* expr) {
                DataType condition = evaluate(expr->m_condition.get());
                if (condition.m_type != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "If condition cannot evaluate to a " + 
                                            Token::to_string(condition.m_type) + ".");
                    return DataType(TokenType::ERROR);
                }
                evaluate(expr->m_then_branch.get());
                if(expr->m_else_branch) evaluate(expr->m_else_branch.get());

                return DataType(TokenType::NIL_TYPE);
            }

            DataType visit(For* expr) {
                evaluate(expr->m_initializer.get());
                DataType condition = evaluate(expr->m_condition.get());
                evaluate(expr->m_update.get());
                if (condition.m_type != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "For loop condition cannot evaluate to a " + 
                                            Token::to_string(condition.m_type) + ".");
                    return DataType(TokenType::ERROR);
                }
                evaluate(expr->m_body.get());

                return DataType(TokenType::NIL_TYPE);
            }

            DataType visit(While* expr) {
                DataType condition = evaluate(expr->m_condition.get());
                if (condition.m_type != TokenType::BOOL_TYPE) {
                    add_error(expr->m_name, "For loop condition cannot evaluate to a " + 
                                            Token::to_string(condition.m_type) + ".");
                    return DataType(TokenType::ERROR);
                }
                evaluate(expr->m_body.get());

                return DataType(TokenType::NIL_TYPE);
            }

            /*
             * Classes
             */
            DataType visit(DeclClass* expr) {
                //put fields in field_sig
                std::unordered_map<std::string, DataType> field_sig;
                for (std::shared_ptr<Expr> e: expr->m_fields) {
                    std::string lexeme = dynamic_cast<DeclVar*>(e.get())->m_name.m_lexeme;
                    DataType dt = evaluate(e.get());
                    field_sig[lexeme] = dt;
                }
                
                //put methods in method_sig
                std::unordered_map<std::string, std::vector<DataType>> method_sig;
                //loop through each method in class
                for (std::shared_ptr<Expr> e: expr->m_methods) {
                    DeclFun* decl_fun = dynamic_cast<DeclFun*>(e.get());
                    std::string lexeme = decl_fun->m_name.m_lexeme;

                    //Disallow methods with same name
                    if (method_sig.count(lexeme) > 0) {
                        add_error(decl_fun->m_name, "Class method " + decl_fun->m_name.to_string() +
                                                    " already defined.");
                        return DataType(TokenType::ERROR);
                    }

                    std::vector<DataType> m_sig;

                    //loop through each parameter in method
                    for (std::shared_ptr<Expr> p: decl_fun->m_parameters) {
                        DeclVar* decl_var = dynamic_cast<DeclVar*>(p.get());
                        m_sig.push_back(DataType(decl_var->m_type.m_type)); 
                    }
                    m_sig.push_back(DataType(decl_fun->m_return_type));
                    method_sig[lexeme] = m_sig;
                }

                m_class_sig.back()[expr->m_name.m_lexeme] = {field_sig, method_sig};

                return DataType(TokenType::NIL_TYPE);
            }

            DataType visit(InstClass* expr) {
                if (!is_declared_class(expr->m_class.m_lexeme)) {
                    add_error(expr->m_name, expr->m_class.m_lexeme + " is not declared.");
                    return DataType(TokenType::ERROR);
                }

                m_var_sig.back()[expr->m_name.m_lexeme] = DataType(TokenType::CUSTOM_TYPE, expr->m_class.m_lexeme);

                return DataType(TokenType::CUSTOM_TYPE, expr->m_class.m_lexeme);
            }

            DataType visit(GetField* expr) {
                //look up instance in m_var_sig
                //use m_lexeme in data type to look up class in m_class_sig
                //check type of field use is trying to access
                return DataType(TokenType::ERROR);
            }

            DataType visit(SetField* expr) {
                /*
                //check if field is declared
                //check the type of field == type of value
                //if so, return that value
                TokenType field_type = get_field_type_from_instance(stmt->m_instance, stmt->m_field);
                TokenType value_t = evaluate(stmt->m_value.get());

                if (field_type != value_t) {
                    throw TypeError(stmt->m_instance, stmt->m_field.to_string() + " type does not match assignment type.");
                }*/
                return DataType(TokenType::ERROR);
            }

            DataType visit(CallMethod* expr) {
                //check that argument count / type is equal to function signature
                //return the return type of the signature
                return DataType(TokenType::ERROR);
            }


    };
}


#endif //ZEBRA_TYPE_CHECKER_H
