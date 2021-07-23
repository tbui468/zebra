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
                        std::cout << "[Line " << m_token.m_line << "] Parsing Error: " << m_message << std::endl;
                    }
            };
        public:
            Parser(const std::vector<Token>& tokens): m_tokens(tokens), m_current(0) {}
            std::vector<std::shared_ptr<Stmt>> parse() {
                std::vector<std::shared_ptr<Stmt>> sl;
                try {
                    while(!match(TokenType::EOFILE)) {
                        sl.push_back(statement());
                    }
                } catch (ParseError& error) {
                    error.print();
                }

                return sl;
            }

            std::shared_ptr<Stmt> statement() {
                if (match(TokenType::PRINT)) return print_statement();
                if (match(TokenType::IF)) return if_statement();
                if (peek(TokenType::LEFT_BRACE)) return block_statement();
                
                throw ParseError(previous(), "Invalid token");
            }

            std::shared_ptr<Stmt> print_statement() {
                std::shared_ptr<Expr> value = expression(); //this needs to go through recursive descent
                std::shared_ptr<Stmt> ret = std::make_shared<Print>(value);
                consume(TokenType::SEMICOLON, "Expect semicolon after expression.");
                return ret;
            }

            std::shared_ptr<Stmt> if_statement() {
                consume(TokenType::LEFT_PAREN, "Expect '(' after keyword 'if'.");
                std::shared_ptr<Expr> condition = expression();
                consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
                std::shared_ptr<Stmt> then_branch = block_statement();

                std::shared_ptr<Stmt> else_branch = nullptr;
                if(match(TokenType::ELSE)) {
                    else_branch = block_statement();
                }
                return std::make_shared<If>(condition, then_branch, else_branch);
            }

            std::shared_ptr<Stmt> block_statement() {
                consume(TokenType::LEFT_BRACE, "Expect '{' to start new block.");
                std::vector<std::shared_ptr<Stmt>> statements;
                while (!match(TokenType::RIGHT_BRACE)) {
                    statements.push_back(statement());
                }

                return std::make_shared<Block>(statements);
            }

            std::shared_ptr<Expr> expression() {
                return logic_or();
            }

            //assignment

            std::shared_ptr<Expr> logic_or() {
                std::shared_ptr<Expr> left = logic_and();
                while(match(TokenType::OR)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = logic_and();
                    left = std::make_shared<Logic>(op, left, right);
                }
            
                return left;             
            }

            std::shared_ptr<Expr> logic_and() {
                std::shared_ptr<Expr> left = equality();
                while(match(TokenType::AND)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = equality();
                    left = std::make_shared<Logic>(op, left, right);
                }
            
                return left;             
            }

            
            //== and !=
            std::shared_ptr<Expr> equality() {
                std::shared_ptr<Expr> left = inequality();
                while(match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = inequality();
                    left = std::make_shared<Logic>(op, left, right);
                }
            
                return left;             
            }            

            //<, <=, >, >=
            std::shared_ptr<Expr> inequality() {
                std::shared_ptr<Expr> left = term();
                while(match(TokenType::LESS) || 
                      match(TokenType::LESS_EQUAL) ||
                      match(TokenType::GREATER) ||
                      match(TokenType::GREATER_EQUAL)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = term();
                    left = std::make_shared<Logic>(op, left, right);
                }
            
                return left;             
            }

            std::shared_ptr<Expr> term() {
                std::shared_ptr<Expr> left = factor();
                while(match(TokenType::PLUS) || match(TokenType::MINUS)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = factor();
                    left = std::make_shared<Binary>(op, left, right);
                }
            
                return left;             
            }

            std::shared_ptr<Expr> factor() {
                std::shared_ptr<Expr> left = unary();
                while(match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = unary();
                    left = std::make_shared<Binary>(op, left, right);
                }
            
                return left;             
            }

            std::shared_ptr<Expr> unary() {
                std::shared_ptr<Expr> right = nullptr;
                while(match(TokenType::MINUS) || match(TokenType::BANG)) {
                    Token op = previous();
                    right = std::make_shared<Unary>(op, unary());
                }

                if(right) {
                    return right;
                } else {
                    return group();
                }
            }

            std::shared_ptr<Expr> group() {
                if(match(TokenType::LEFT_PAREN)) {
                    Token t = previous();
                    std::shared_ptr<Expr> expr = expression();
                    consume(TokenType::RIGHT_PAREN, "Expect closing parenthesis");
                    return std::make_shared<Group>(t, expr);
                }

                return primary();
            }

            std::shared_ptr<Expr> primary() {
                if (match(TokenType::FLOAT)) {
                    return std::make_shared<Literal>(previous());
                }else if(match(TokenType::INT)) {
                    return std::make_shared<Literal>(previous());
                }else if(match(TokenType::STRING)) {
                    return std::make_shared<Literal>(previous());
                }else if(match(TokenType::TRUE)) {
                    return std::make_shared<Literal>(previous());
                }else if(match(TokenType::FALSE)) {
                    return std::make_shared<Literal>(previous());
                }else if(match(TokenType::NIL)) {
                    return std::make_shared<Literal>(previous());
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

            bool peek(TokenType type) {
                return m_tokens.at(m_current).m_type == type;
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
