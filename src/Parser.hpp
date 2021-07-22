#ifndef ZEBRA_PARSER_H
#define ZEBRA_PARSER_H

#include <vector>
#include <iostream>
#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

namespace zebra {


    class Parser {
        private:
            std::vector<Token> m_tokens;
            int m_current;
            class ParseError {
                private:
                    Token m_token;
                    std::string m_message;
                public:
                    ParseError(Token token, const std::string& message): m_token(token), m_message(message){}
                    ~ParseError() {}
                    void print() {
                        std::cout << "[Line " << m_token.m_line << "] Error: " << m_message << std::endl;
                    }
            };
        public:
            Parser(const std::vector<Token>& tokens): m_tokens(tokens), m_current(0) {}
            std::vector<Stmt*> parse() {
                std::vector<Stmt*> sl;
                try {
                    while(!match(TokenType::EOFILE)) {
                        sl.push_back(statement());
                    }
                } catch (ParseError& error) {
                    error.print();
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
                }else if(match(TokenType::TRUE)) {
                    return new Literal(previous());
                }else if(match(TokenType::FALSE)) {
                    return new Literal(previous());
                }else if(match(TokenType::NIL)) {
                    return new Literal(previous());
                }
                throw ParseError(previous(), "Expecting an expression.");
            }

            bool match(TokenType type) {
                if(m_tokens.at(m_current).m_type == type) {
                    m_current++;
                    return true;
                }
                return false;
            }

            const Token& previous() {
                return m_tokens.at(m_current - 1);
            }

            void consume(TokenType type, const std::string& message) {
                if (!match(type)) {
                    Token t = previous();
                    throw ParseError(t, message);
                }
            }

    };

}


#endif //ZEBRA_PARSER_H
