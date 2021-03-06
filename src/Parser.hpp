#ifndef ZEBRA_PARSER_H
#define ZEBRA_PARSER_H

#include <vector>
#include <iostream>
#include "Token.hpp"
#include "Expr.hpp"
#include "ResultCode.hpp"

namespace zebra {

    struct ParseError {
        Token m_token;
        std::string m_message;
        ParseError(Token token, const std::string& message): m_token(token), m_message(message) {}
        void print() {
            std::cout << "[Line " << m_token.m_line << "] Parsing Error: " << m_message << std::endl;
        }
    };


    class Parser {
        private:
            std::vector<Token> m_tokens;
            int m_current;
            //Note: doesn't check if return statement if valid (eg outside of function) - resolver should do that in next phase
            //Used for type checking for function and return
            TokenType m_return_type = TokenType::NIL_TYPE;
            bool m_had_return_flag = false;
            std::vector<ParseError> m_errors;

            bool m_error_flag {false};

        public:
            Parser(const std::vector<Token>& tokens): m_tokens(tokens), m_current(0) {}

            ResultCode parse(std::vector<std::shared_ptr<Expr>>& ast) {
                while(!match(TokenType::EOFILE)) {
                    std::shared_ptr<Expr> expr = expression();
                    if (!m_error_flag) {
                        ast.push_back(expr);
                    } else {
                        synchronize();
                        m_error_flag = false;
                    }
                }

                if (m_errors.empty()) {
                    return ResultCode::SUCCESS;
                } else {
                    return ResultCode::FAILED;
                }
            }

            std::vector<ParseError> get_errors() const {
                return m_errors;
            }

        private:

            void add_error(Token token, const std::string& message) {
                m_errors.emplace_back(token, message);
                m_error_flag = true;
            }


            /*
             * Expressions
             */

            std::shared_ptr<Expr> expression() {
                return declare_assign();
            }


            //using two peeks here since an IDENTIFIER may be a function call, which has highest precedence
            std::shared_ptr<Expr> declare_assign() {
                //variable assignment
                if (peek_two(TokenType::IDENTIFIER, TokenType::EQUAL)) {
                    match(TokenType::IDENTIFIER);
                    Token identifier = previous();
                    match(TokenType::EQUAL);
                    std::shared_ptr<Expr> value = expression();
                    return std::make_shared<SetVar>(identifier, Token(TokenType::NIL), value);
                //instance field assignment
                } else if (peek_four(TokenType::IDENTIFIER, TokenType::DOT, TokenType::IDENTIFIER, TokenType::EQUAL)) {
                    match(TokenType::IDENTIFIER);
                    Token env = previous();
                    match(TokenType::DOT);
                    match(TokenType::IDENTIFIER);
                    Token field = previous();
                    match(TokenType::EQUAL);

                    std::shared_ptr<Expr> value = expression();

                    return std::make_shared<SetVar>(field, env, value);
                } else if (peek_two(TokenType::IDENTIFIER, TokenType::COLON)) { //variable
                    match(TokenType::IDENTIFIER);
                    Token identifier = previous();
                    match(TokenType::COLON);
                    
                    //TODO: this is ugly and error-prone - find a nicer way to do this
                    match(TokenType::BOOL_TYPE);
                    match(TokenType::INT_TYPE);
                    match(TokenType::FLOAT_TYPE);
                    match(TokenType::STRING_TYPE);
                    match(TokenType::IDENTIFIER);
                    Token type = previous();

                    if (type.m_type == TokenType::COLON) {
                        add_error(type, "Invalid data type.");
                    }

                    //check for possible assignment
                    std::shared_ptr<Expr> value = nullptr;
                    if(match(TokenType::EQUAL)) {
                        value = expression();
                    }

                    return std::make_shared<DeclVar>(identifier, type, value);
                }

                return logic_or();
            }

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

            //right associative
            std::shared_ptr<Expr> unary() {
                if(match(TokenType::MINUS) || match(TokenType::BANG)) {
                    Token op = previous();
                    std::shared_ptr<Expr> right = unary();
                    return std::make_shared<Unary>(op, right);
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
                }else if(peek_four(TokenType::IDENTIFIER, TokenType::DOT, TokenType::IDENTIFIER, TokenType::LEFT_PAREN)) {
                    match(TokenType::IDENTIFIER);
                    Token env = previous();
                    match(TokenType::DOT);
                    match(TokenType::IDENTIFIER);
                    Token fun = previous();
                    match(TokenType::LEFT_PAREN);
                    std::vector<std::shared_ptr<Expr>> arguments;
                    while (!match(TokenType::RIGHT_PAREN)) {
                        arguments.push_back(expression());
                        match(TokenType::COMMA);
                    }
                    return std::make_shared<CallFun>(fun, env, arguments);
                }else if(peek_three(TokenType::IDENTIFIER, TokenType::DOT, TokenType::IDENTIFIER)) {
                    match(TokenType::IDENTIFIER);
                    Token env = previous();
                    match(TokenType::DOT);
                    match(TokenType::IDENTIFIER);
                    Token field = previous();

                    return std::make_shared<GetVar>(field, env);
                }else if(peek_two(TokenType::IDENTIFIER, TokenType::LEFT_PAREN)) {
                    match(TokenType::IDENTIFIER);
                    Token identifier = previous();
                    consume(TokenType::LEFT_PAREN, "Expected '(' after function identifier");

                    std::vector<std::shared_ptr<Expr>> arguments;
                    while(!match(TokenType::RIGHT_PAREN)) {
                        arguments.emplace_back(expression());
                        match(TokenType::COMMA);
                    }

                    return std::make_shared<CallFun>(identifier, Token(TokenType::NIL), arguments);
                }else if(match(TokenType::LEFT_PAREN)) {
                    Token t = previous();
                    std::shared_ptr<Expr> expr = expression();

                    consume(TokenType::RIGHT_PAREN, "Expect closing parenthesis");
                    return std::make_shared<Group>(t, expr);
                }else if(match(TokenType::LEFT_BRACE)) {
                    Token name = previous();
                    std::vector<std::shared_ptr<Expr>> expressions;
                    while (!match(TokenType::RIGHT_BRACE)) {
                        expressions.push_back(expression());
                    }

                    return std::make_shared<Block>(name, expressions);
                } else if(match(TokenType::IF)) {
                    Token name = previous();
                    std::shared_ptr<Expr> condition = expression();
                    std::shared_ptr<Expr> then_branch = expression();

                    std::shared_ptr<Expr> else_branch = nullptr;
                    if(match(TokenType::ELSE)) {
                        else_branch = expression();
                    }
                    return std::make_shared<If>(name, condition, then_branch, else_branch);
                } else if(match(TokenType::FOR)) {
                    Token name = previous();

                    std::shared_ptr<Expr> initializer;
                    if(!match(TokenType::COMMA)) {
                        initializer = expression();
                        consume(TokenType::COMMA, "Expecting comma after initializer.");
                    }

                    std::shared_ptr<Expr> condition;
                    if(!match(TokenType::COMMA)) {
                        condition = expression();
                        consume(TokenType::COMMA, "Expecting comma after condition.");
                    }

                    std::shared_ptr<Expr> update;
                    if(!match(TokenType::LEFT_BRACE)) {
                        update = expression();
                    }

                    std::shared_ptr<Expr> body = expression();

                    return std::make_shared<For>(name, initializer, condition, update, body);
                } else if(match(TokenType::WHILE)) {
                    Token name = previous();
                    std::shared_ptr<Expr> condition = expression();
                    std::shared_ptr<Expr> body = expression();
                    return std::make_shared<While>(name, condition, body);
                } else if(peek_three(TokenType::IDENTIFIER, TokenType::COLON_COLON, TokenType::LEFT_PAREN)) {
                    match(TokenType::IDENTIFIER);
                    Token identifier = previous();
                    match(TokenType::COLON_COLON);

                    //parameter list
                    consume(TokenType::LEFT_PAREN, "Expect '(' before function parameters.");

                    std::vector<std::shared_ptr<Expr>> parameters;
                    while(!match(TokenType::RIGHT_PAREN)) {
                        match(TokenType::IDENTIFIER);
                        Token name = previous();

                        consume(TokenType::COLON, "Expect colon after variable identifier.");

                        //TODO: this is ugly and error-prone - find a nicer way to do this
                        match(TokenType::BOOL_TYPE);
                        match(TokenType::INT_TYPE);
                        match(TokenType::FLOAT_TYPE);
                        match(TokenType::STRING_TYPE);
                        match(TokenType::FUN_TYPE);
                        match(TokenType::IDENTIFIER);
                        Token type = previous();

                        if (type.m_type == TokenType::COLON) {
                            add_error(type, "Invalid parameter type.");
                        }

                        parameters.emplace_back(std::make_shared<DeclVar>(name, type, nullptr));
                        match(TokenType::COMMA);                    
                    }                

                    consume(TokenType::RIGHT_ARROW, "Expect '->' and return type after parameter declaration.");
                    match(TokenType::BOOL_TYPE);
                    match(TokenType::INT_TYPE);
                    match(TokenType::FLOAT_TYPE);
                    match(TokenType::STRING_TYPE);
                    match(TokenType::FUN_TYPE);
                    match(TokenType::IDENTIFIER);
                    if (previous().m_type == TokenType::RIGHT_ARROW) {
                        m_return_type = TokenType::NIL_TYPE;
                    } else {
                        m_return_type = previous().m_type; 
                    }

                    consume(TokenType::LEFT_BRACE, "Expect '{' to start new block.");

                    Token name = previous(); //block name

                    //setting flag to default false, will be set to true if return statement in body
                    m_had_return_flag = false;

                    std::vector<std::shared_ptr<Expr>> expressions;
                    while (!match(TokenType::RIGHT_BRACE)) {
                        expressions.push_back(expression());
                    }

                    if (m_return_type != TokenType::NIL_TYPE && !m_had_return_flag) {
                        add_error(identifier, "Expect return statement.");
                    }

                    std::shared_ptr<Expr> body = std::make_shared<Block>(name, expressions);
                    return std::make_shared<DeclFun>(identifier, parameters, m_return_type, body);
                } else if(peek_three(TokenType::IDENTIFIER, TokenType::COLON_COLON, TokenType::CLASS)) {
                    match(TokenType::IDENTIFIER);
                    Token name = previous();
                    match(TokenType::COLON_COLON);
                    match(TokenType::CLASS);

                    Token base;
                    if (peek_two(TokenType::LESS, TokenType::IDENTIFIER)) {
                        match(TokenType::LESS);
                        match(TokenType::IDENTIFIER);
                        base = previous();
                    }

                    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");
                    std::vector<std::shared_ptr<Expr>> fields;
                    std::vector<std::shared_ptr<Expr>> methods;
                    while (!match(TokenType::RIGHT_BRACE)) {
                        std::shared_ptr<Expr> decl = expression();
                        
                        //either a DeclVar or DeclFun
                        if (dynamic_cast<DeclVar*>(decl.get())) {
                            fields.push_back(decl);
                        } else {
                            methods.push_back(decl);
                        }
                    }

                    return std::make_shared<DeclClass>(name, base, fields, methods);
                } else if(match(TokenType::RIGHT_ARROW)) {
                    m_had_return_flag = true;
                    Token name = previous();
                    std::shared_ptr<Expr> value = nullptr;
                    if (m_return_type != TokenType::NIL_TYPE) {
                        value = expression();
                    }
                    return std::make_shared<Return>(name, value);
                } else if (match(TokenType::IDENTIFIER)) {
                    return std::make_shared<GetVar>(previous(), Token(TokenType::NIL));
                }

                add_error(previous(), "Expecting an expression.");
                return nullptr;
            }


            bool match(TokenType type) {
                if(m_tokens.at(m_current).m_type == type) {
                    m_current++;
                    return true;
                }
                return false;
            }

            bool peek_one(TokenType type) {
                if (m_current >= m_tokens.size()) return false;
                return m_tokens.at(m_current).m_type == type;
            }
            bool peek_two(TokenType type1, TokenType type2) {
                if (m_current + 1 >= m_tokens.size()) return false;
                return peek_one(type1) && (m_tokens.at(m_current + 1).m_type == type2);
            }

            bool peek_three(TokenType type1, TokenType type2, TokenType type3) {
                if (m_current + 2 >= m_tokens.size()) return false;
                return peek_two(type1, type2) && (m_tokens.at(m_current + 2).m_type == type3);
            }

            bool peek_four(TokenType type1, TokenType type2, TokenType type3, TokenType type4) {
                if (m_current + 3 >= m_tokens.size()) return false;
                return peek_three(type1, type2, type3) && (m_tokens.at(m_current + 3).m_type == type4);
            }

            const Token& previous() {
                return m_tokens.at(m_current - 1);
            }

            void consume(TokenType type, const std::string& message) {
                if (!match(type)) {
                    Token t = previous();
                    add_error(t, message);
                }
            }

            void synchronize() {
                while (!peek_one(TokenType::EOFILE)) {
                    if (match(TokenType::RIGHT_BRACE) || match(TokenType::SEMICOLON)) {
                        break;
                    }
                    m_current++;
                }
            }

    };

}


#endif //ZEBRA_PARSER_H
