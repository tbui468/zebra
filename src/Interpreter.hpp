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
            std::vector<Stmt*> m_statements;
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
            Interpreter(const std::vector<Stmt*> statements): m_statements(statements) {}
            ~Interpreter() {}
            void run() {
                try {
                    for(Stmt* s: m_statements) {
                        execute(*s);
                    }
                } catch(RuntimeError& e) {
                    e.print();
                }
            }
            void execute(Stmt& stmt) {
                stmt.accept(*this);
            }

            void visit(Print& stmt) {
                Object* value = stmt.m_value->accept(*this);
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
                delete value;
            }

            void visit(If& stmt) {
                Object* condition = evaluate(*(stmt.m_condition));
                if(condition->is_truthy()) {
                    execute(*(stmt.m_body));                    
                }

                delete condition;
            }

            void visit(Block& stmt) {
                for(Stmt* s: stmt.m_statements) {
                    execute(*s);  
                }    
            }

            Object* evaluate(Expr& expr) {
                return expr.accept(*this);
            }

            Object* visit(Unary& expr) {
                Object* right = expr.m_right->accept(*this);

                if(expr.m_op.m_type == TokenType::MINUS) {
                    if(right->is_float()) {
                        float a = right->get_float();
                        delete right;
                        return new Object(-a);
                    }else if(right->is_int()) {
                        int a = right->get_int();
                        delete right;
                        return new Object(-a);
                    }
                }else if(expr.m_op.m_type == TokenType::BANG) {
                    bool b = false;
                    if(right->is_truthy()) {
                       b = true; 
                    }
                    delete right;
                    return new Object(!b);
                }

                throw RuntimeError(expr.m_op, "That invalid unary operator.");
            }

            Object* visit(Binary& expr) {
                //if left and right aren't of the same type (string, float or int), throw error
                Object* left = expr.m_left->accept(*this);
                Object* right = expr.m_right->accept(*this);
                if(left->is_int() && right->is_int()) {
                    int a = left->get_int();
                    int b = right->get_int();
                    delete left;
                    delete right;
                    switch(expr.m_op.m_type) {
                        case TokenType::PLUS: return new Object(a + b);
                        case TokenType::MINUS: return new Object(a - b);
                        case TokenType::STAR: return new Object(a * b);
                        case TokenType::SLASH: return new Object(a / b);
                        case TokenType::MOD: return new Object(a % b);
                    }
                    throw RuntimeError(expr.m_op, "That operation can't be performed on ints.");
                }
                if(left->is_float() && right->is_float()) {
                    float a = left->get_float();
                    float b = right->get_float();
                    delete left;
                    delete right;
                    switch(expr.m_op.m_type) {
                        case TokenType::PLUS: return new Object(a + b);
                        case TokenType::MINUS: return new Object(a - b);
                        case TokenType::STAR: return new Object(a * b);
                        case TokenType::SLASH: return new Object(a / b);
                    }
                    throw RuntimeError(expr.m_op, "That operation can't be performed on floats.");
                }
                if(left->is_string() && right->is_string()) {
                    std::string a = left->get_string();
                    std::string b = right->get_string();
                    delete left;
                    delete right;
                    switch(expr.m_op.m_type) {
                        case TokenType::PLUS: return new Object(a + b);
                    }
                    throw RuntimeError(expr.m_op, "That operation can't be performed on strings.");
                }

                throw RuntimeError(expr.m_op, "Types in expression must match.");
            }
            Object* visit(Group& expr) {
                return expr.m_expr->accept(*this);
            }
            Object* visit(Literal& expr) {
                switch(expr.m_token.m_type) {
                    case TokenType::FLOAT:
                        return new Object(stof(expr.m_token.m_lexeme));
                    case TokenType::INT:
                        return new Object(stoi(expr.m_token.m_lexeme));
                    case TokenType::STRING:
                        return new Object(expr.m_token.m_lexeme);
                    case TokenType::TRUE:
                        return new Object(true);
                    case TokenType::FALSE:
                        return new Object(false);
                    case TokenType::NIL:
                        return new Object();
                }
            }


    };

}


#endif //ZEBRA_INTERPRETER_H
