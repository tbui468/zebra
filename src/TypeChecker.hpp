#ifndef ZEBRA_TYPE_CHECKER_H
#define ZEBRA_TYPE_CHECKER_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include "TokenType.hpp"
#include "DataType.hpp"
#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {


    class TypeChecker: public ExprDataTypeVisitor, public StmtVoidVisitor {
        private:
            std::unordered_map<std::string, DataType> m_types;
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
            TypeChecker() {}
            ~TypeChecker() {}
            void execute(std::shared_ptr<Stmt> stmt) {
                stmt->accept(*this);
            }
            bool check(const std::vector<std::shared_ptr<Stmt>>& ast) {
                try {
                    for(std::shared_ptr<Stmt> s: ast) {
                        execute(s);
                    }
                } catch (TypeError& e) {
                    e.print();
                    return false;
                }

                return true;
            }
        private:

            DataType evaluate(std::shared_ptr<Expr> expr) {
                return expr->accept(*this);
            }

            /*
             * Statements
             */

            void visit(std::shared_ptr<Print> stmt) {
                DataType type = evaluate(stmt->m_value);                
            }
            void visit(std::shared_ptr<If> stmt) {
                DataType type = evaluate(stmt->m_condition);
                execute(stmt->m_then_branch);
                if(stmt->m_else_branch) execute(stmt->m_else_branch);
            }
            void visit(std::shared_ptr<Block> stmt) {
                for(std::shared_ptr<Stmt> s: stmt->m_statements) {
                    execute(s);
                }
            }

            void visit(std::shared_ptr<AssignStmt> stmt) {
                /*
                if(m_types.count(stmt->m_name.m_lexeme) == 0) {
                    throw TypeError(stmt->m_name, "Variable " + stmt->m_name.to_string() + " not declared.");
                }*/
                DataType type = m_types[stmt->m_name.m_lexeme];
                DataType expr_type = evaluate(stmt->m_value);

                if(type != expr_type) {
                    throw TypeError(stmt->m_name, "Right hand expression must evaluate to type " + stmt->m_name.to_string() + ".");
                }
            }

            void visit(std::shared_ptr<VarDecl> stmt) {
                DataType type;
                switch(stmt->m_type.m_type) {
                    case TokenType::BOOL_TYPE:
                        type = DataType::BOOL;
                        break;
                    case TokenType::INT_TYPE:
                        type = DataType::INT;
                        break;
                    case TokenType::FLOAT_TYPE:
                        type = DataType::FLOAT;
                        break;
                    case TokenType::STRING_TYPE:
                        type = DataType::STRING;
                        break;
                }
                m_types[stmt->m_name.m_lexeme] = type;
                DataType expr_type = evaluate(stmt->m_value);
                if(type != expr_type) {
                    throw TypeError(stmt->m_name, "Right hand expression must evaluate to type " + stmt->m_name.to_string() + ".");
                }
            }

            void visit(std::shared_ptr<While> stmt) {
                DataType type = evaluate(stmt->m_condition);
                if (type != DataType::BOOL) {
                    throw TypeError(stmt->m_name, "Condition must evaluate to a boolean.");
                }
                execute(stmt->m_body);
            }

            /*
             * Expressions
             */
            DataType visit(std::shared_ptr<Unary> expr) {
                TokenType op_type = expr->m_op.m_type;
                DataType right_type = evaluate(expr->m_right);

                switch(op_type) {
                    case TokenType::MINUS:
                        if(right_type == DataType::INT || right_type == DataType::FLOAT) return right_type;
                        break;
                    case TokenType::BANG:
                        if(right_type == DataType::BOOL) return right_type;
                        break;
                }

                throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on this data type.");
            }

            DataType visit(std::shared_ptr<Binary> expr) {
                DataType left = evaluate(expr->m_left);
                DataType right = evaluate(expr->m_right);

                if(left == DataType::BOOL || right == DataType::BOOL)
                    throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on booleans.");

                if(left == right) return left;

                throw TypeError(expr->m_op, "Cannot use " + expr->m_op.to_string() + " operator on different data types. Explicity cast types if necessary.");
            }

            DataType visit(std::shared_ptr<Group> expr) {
                return evaluate(expr->m_expr);
            }

            DataType visit(std::shared_ptr<Literal> expr) {
                switch(expr->m_token.m_type) {
                    case TokenType::TRUE:
                    case TokenType::FALSE:
                        return DataType::BOOL;
                    case TokenType::FLOAT:
                        return DataType::FLOAT;
                    case TokenType::INT:
                        return DataType::INT;
                    case TokenType::STRING:
                        return DataType::STRING;
                }
                throw TypeError(expr->m_token, expr->m_token.to_string() + " token not valid.");
            }

            DataType visit(std::shared_ptr<Logic> expr) {
                DataType left = evaluate(expr->m_left);
                DataType right = evaluate(expr->m_right);

                bool inequality = expr->m_op.m_type == TokenType::LESS ||
                                   expr->m_op.m_type == TokenType::LESS_EQUAL ||
                                   expr->m_op.m_type == TokenType::GREATER ||
                                   expr->m_op.m_type == TokenType::GREATER_EQUAL;

                if ((left == DataType::BOOL || right == DataType::BOOL) && inequality) {
                    throw TypeError(expr->m_op, "Can't use " + expr->m_op.to_string() + " with inequalities.");
                }

                if ((left == DataType::STRING || right == DataType::STRING) && inequality) {
                    throw TypeError(expr->m_op, "Can't use " + expr->m_op.to_string() + " with inequalities.");
                }
                   
                if(left == right) return DataType::BOOL;

                throw TypeError(expr->m_op, "Inputs to logical operator must be of same type.");
            }


            DataType visit(std::shared_ptr<AssignExpr> expr) {
                if(m_types.count(expr->m_name.m_lexeme) == 0) {
                    throw TypeError(expr->m_name, "Variable not declared.");
                }

                DataType type = m_types[expr->m_name.m_lexeme];
                DataType expr_type = evaluate(expr->m_value);

                if(type != expr_type) {
                    throw TypeError(expr->m_name, "Right hand expression must evaluate to type " + expr->m_name.to_string() + ".");
                }
            }

            DataType visit(std::shared_ptr<Variable> expr) {
                /*
                if(m_types.count(expr->m_name.m_lexeme) == 0) {
                    throw TypeError(expr->m_name, "Variable not declared.");
                }*/

                return m_types[expr->m_name.m_lexeme];
            }


    };
}


#endif //ZEBRA_TYPE_CHECKER_H
