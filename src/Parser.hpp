#ifndef ZEBRA_PARSER_H
#define ZEBRA_PARSER_H

#include <stdio.h>
#include "Token.hpp"
#include "TokenArray.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "StmtList.hpp"

namespace zebra {


    class Parser {
        private:
            TokenArray* m_tokens;
            int m_current;
            class ParseError {
                private:
                    Token m_token;
                    const char* m_message;
                public:
                    ParseError(Token token, const char* message): m_token(token), m_message(message){}
                    ~ParseError() {}
                    void print() {
                        printf("[Line %d] Error: %s", m_token.m_line, m_message);
                    }
            };
        public:

            static void print(Stmt* node) {
                if(dynamic_cast<Print*>(node)) {
                    printf("( Print ");
                    print(dynamic_cast<Print*>(node)->m_value);
                    printf(" )");
                } else {
                } 
            }
            static void print(Expr* node) {
                if(dynamic_cast<Literal*>(node)) {
                    dynamic_cast<Literal*>(node)->m_token.print();
                } else if (dynamic_cast<Binary*>(node)) {
                    Binary* binary = dynamic_cast<Binary*>(node);
                    printf("( ");
                    binary->m_op.print();
                    printf(" ");
                    print(binary->m_left);
                    printf(" ");
                    print(binary->m_right);
                    printf(" )");
                } else if (dynamic_cast<Unary*>(node)) {
                    Unary* unary = dynamic_cast<Unary*>(node);
                    printf("( ");
                    unary->m_op.print();
                    printf(" ");
                    print(unary->m_right);
                    printf(" )");
                }else if(dynamic_cast<Group*>(node)) {
                    printf("( Group ");
                    print(dynamic_cast<Group*>(node)->m_expr);
                    printf(" )");
                }
            }

        public:
            Parser(TokenArray* token_array): m_tokens(token_array), m_current(0) {}
            StmtList* parse() {
                StmtList* sl = new StmtList();
                try {
                    while(!match(TokenType::EOFILE)) {
                        sl->add(statement());
                    }
                } catch (ParseError& error) {
                    error.print();
                    delete sl;
                    return nullptr;
                }

                return sl;
            }


            Stmt* statement() {
                if (match(TokenType::PRINT)) return print_statement();
                
                throw ParseError(previous(), "Invalid token");
            }

            Stmt* print_statement() {
                Expr* value = expression(); //this needs to go through recursive descent
                Stmt* ret = new Print(value);
                consume(TokenType::SEMICOLON, "Expect semicolon after expression.");
                return ret;
            }

            Expr* expression() {
                return term();
            }

            //assignment

            //or

            //and
            
            //equal !equal

            //four inequalities

            Expr* term() {
                Expr* left = factor();
                while(match(TokenType::PLUS) || match(TokenType::MINUS)) {
                    Token op = previous();
                    Expr* right = factor();
                    left = new Binary(op, left, right);
                }
            
                return left;             
            }

            Expr* factor() {
                Expr* left = unary();
                while(match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD)) {
                    Token op = previous();
                    Expr* right = unary();
                    left = new Binary(op, left, right);
                }
            
                return left;             
            }

            Expr* unary() {
                Expr* right = nullptr;
                while(match(TokenType::MINUS) || match(TokenType::BANG)) {
                    Token op = previous();
                    right = new Unary(op, unary());
                }

                if(right) {
                    return right;
                } else {
                    return group();
                }
            }

            Expr* group() {
                if(match(TokenType::LEFT_PAREN)) {
                    Token t = previous();
                    Expr* expr = expression();
                    consume(TokenType::RIGHT_PAREN, "Expect closing parenthesis");
                    return new Group(t, expr);
                }

                return primary();
            }

            Expr* primary() {
                if (match(TokenType::FLOAT)) {
                    return new Literal(previous());
                }else if(match(TokenType::INT)) {
                    return new Literal(previous());
                }else if(match(TokenType::STRING)) {
                    return new Literal(previous());
                }
                throw ParseError(previous(), "Expecting an expression.");
            }

            bool match(TokenType type) {
                if(m_tokens->at(m_current).m_type == type) {
                    m_current++;
                    return true;
                }
                return false;
            }

            const Token& previous() {
                return m_tokens->at(m_current - 1);
            }

            void consume(TokenType type, const char* message) {
                if (!match(type)) {
                    Token t = previous();
                    throw ParseError(t, message);
                }
            }

    };

}


#endif //ZEBRA_PARSER_H
