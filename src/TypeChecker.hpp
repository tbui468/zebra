#ifndef ZEBRA_TYPE_CHECKER_H
#define ZEBRA_TYPE_CHECKER_H

#include <vector>
#include <unordered_map>
#include <iostream>

#include "TokenType.hpp"
#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {


    class TypeChecker: public ExprTokenTypeVisitor, public StmtVoidVisitor {
        private:
            std::vector<std::unordered_map<std::string, Stmt*>> m_variables;
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

        public:
            TypeChecker() {
                m_variables.emplace_back(std::unordered_map<std::string, Stmt*>()); 
            }
            ~TypeChecker() {}
            void execute(Stmt* stmt) {
                stmt->accept(*this);
            }
            bool check(const std::vector<std::shared_ptr<Stmt>>& ast) {
                try {
                    for(std::shared_ptr<Stmt> s: ast) {
                        execute(s.get());
                    }
                } catch (TypeError& e) {
                    e.print();
                    return false;
                }

                return true;
            }
        private:
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
                } else {
                    return dynamic_cast<FunDecl*>(var)->m_type.m_type;
                }
            }

            TokenType evaluate(Expr* expr) {
                return expr->accept(*this);
            }

            /*
             * Statements
             */

            void visit(Print* stmt) {
                TokenType type = evaluate(stmt->m_value.get());                
                if (type == TokenType::NIL_TYPE) {
                    throw TypeError(stmt->m_name, "Cannot print nil type.");
                }
            }
            void visit(If* stmt) {
                TokenType type = evaluate(stmt->m_condition.get());
                execute(stmt->m_then_branch.get());
                if(stmt->m_else_branch) execute(stmt->m_else_branch.get());
            }
            void visit(Block* stmt) {
                m_variables.emplace_back(std::unordered_map<std::string, Stmt*>());
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    execute(s.get());
                }
                m_variables.pop_back();
            }

            void visit(While* stmt) {
                TokenType type = evaluate(stmt->m_condition.get());
                if (type != TokenType::BOOL_TYPE) {
                    throw TypeError(stmt->m_name, "Condition must evaluate to a boolean.");
                }
                execute(stmt->m_body.get());
            }

            void visit(For* stmt) {
                execute(stmt->m_initializer.get());
                TokenType con_type = evaluate(stmt->m_condition.get());
                TokenType up_type = evaluate(stmt->m_update.get());
                if (con_type != TokenType::BOOL_TYPE) {
                    throw TypeError(stmt->m_name, "Condition must evaluate to a boolean.");
                }
                execute(stmt->m_body.get());
            }


            void visit(Return* stmt) {
                TokenType expr_type = evaluate(stmt->m_value.get());
                if(expr_type != stmt->m_return_type.m_type) {
                    throw TypeError(stmt->m_name, "Return type must match return type in function declaration.");
                }
            }

            void visit(FunDecl* stmt) {
                m_variables.back()[stmt->m_name.m_lexeme] = stmt;
                for(std::shared_ptr<Stmt> s: stmt->m_parameters) {
                    VarDecl* var_decl = dynamic_cast<VarDecl*>(s.get());
                    m_variables.back()[var_decl->m_name.m_lexeme] = s.get();
                }

                execute(stmt->m_body.get());
            }

            void visit(VarDecl* stmt) {
                if(stmt->m_value) {
                    TokenType expr_type = evaluate(stmt->m_value.get());
                    if(stmt->m_type.m_type != expr_type) {
                        throw TypeError(stmt->m_name, "Right hand expression must evaluate to type " + stmt->m_name.to_string() + ".");
                    }
                } else {
                    throw TypeError(stmt->m_name, stmt->m_name.to_string() + " must be defined at declaration.");
                }

                m_variables.back()[stmt->m_name.m_lexeme] = stmt; //put indentifier in env. variables
            }

            void visit(Assign* stmt) {
                TokenType type = get_type(stmt->m_name);

                TokenType expr_type = evaluate(stmt->m_value.get());

                if(type != expr_type) {
                    throw TypeError(stmt->m_name, "Right hand expression must evaluate to type " + stmt->m_name.to_string() + ".");
                }
            }

            void visit(AssignField* stmt) {
                TokenType field_type = get_field_type_from_instance(stmt->m_instance, stmt->m_field);
                TokenType value_t = evaluate(stmt->m_value.get());

                if (field_type != value_t) {
                    throw TypeError(stmt->m_instance, stmt->m_field.to_string() + " type does not match assignment type.");
                }
            }

            void visit(Call* stmt) {
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
                }
            }

            void visit(StructDecl* stmt) {
                m_variables.back()[stmt->m_name.m_lexeme] = stmt;
            }

            void visit(StructInst* stmt) {
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
                }
            }


            /*
             * Expressions
             */
            TokenType visit(Unary* expr) {
                TokenType right_type = evaluate(expr->m_right.get());

                switch(expr->m_op.m_type) {
                    case TokenType::MINUS:
                        if(right_type == TokenType::INT_TYPE || right_type == TokenType::FLOAT_TYPE) return right_type;
                        break;
                    case TokenType::BANG:
                        if(right_type == TokenType::BOOL_TYPE) return right_type;
                        break;
                }

                throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on this data type.");
            }

            TokenType visit(Binary* expr) {
                TokenType left = evaluate(expr->m_left.get());
                TokenType right = evaluate(expr->m_right.get());

                if(left == TokenType::BOOL_TYPE || right == TokenType::BOOL_TYPE)
                    throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on booleans.");

                if(left == right) return left;

                throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on different data types. Explicity cast types if necessary.");
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
                    case TokenType::NIL:
                        return TokenType::NIL_TYPE;
                }
                throw TypeError(expr->m_token, expr->m_token.to_string() + " token not valid.");
            }

            TokenType visit(Logic* expr) {
                TokenType left = evaluate(expr->m_left.get());
                TokenType right = evaluate(expr->m_right.get());

                bool inequality = expr->m_op.m_type == TokenType::LESS ||
                                   expr->m_op.m_type == TokenType::LESS_EQUAL ||
                                   expr->m_op.m_type == TokenType::GREATER ||
                                   expr->m_op.m_type == TokenType::GREATER_EQUAL;

                if ((left == TokenType::BOOL_TYPE || right == TokenType::BOOL_TYPE) && inequality) {
                    throw TypeError(expr->m_op, "Can't use " + expr->m_op.to_string() + " with inequalities.");
                }

                if ((left == TokenType::STRING_TYPE || right == TokenType::STRING_TYPE) && inequality) {
                    throw TypeError(expr->m_op, "Can't use " + expr->m_op.to_string() + " with inequalities.");
                }
                   
                if(left == right) return TokenType::BOOL_TYPE;

                throw TypeError(expr->m_op, "Inputs to logical operator must be of same type.");
            }

            TokenType visit(Variable* expr) {
                return get_type(expr->m_name);
            }

            //Return the type of the assignment/declartion
            TokenType visit(StmtExpr* expr) {
                execute(expr->m_stmt.get());

                if (dynamic_cast<Assign*>(expr->m_stmt.get())) {
                    return get_type(dynamic_cast<Assign*>(expr->m_stmt.get())->m_name);
                }

                if (dynamic_cast<AssignField*>(expr->m_stmt.get())) {
                    AssignField* stmt = dynamic_cast<AssignField*>(expr->m_stmt.get());
                    return get_field_type_from_instance(stmt->m_instance, stmt->m_field);
                }

                if (dynamic_cast<VarDecl*>(expr->m_stmt.get())) {
                    return get_type(dynamic_cast<VarDecl*>(expr->m_stmt.get())->m_name);
                }

                if (dynamic_cast<FunDecl*>(expr->m_stmt.get())) {
                    return get_type(dynamic_cast<FunDecl*>(expr->m_stmt.get())->m_name);
                }

                if (dynamic_cast<Call*>(expr->m_stmt.get())) {
                    return get_type(dynamic_cast<Call*>(expr->m_stmt.get())->m_name);
                }
            }


            TokenType visit(Access* expr) {
                return get_field_type_from_instance(expr->m_instance, expr->m_field);
            }


    };
}


#endif //ZEBRA_TYPE_CHECKER_H
