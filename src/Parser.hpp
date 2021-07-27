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
            //Note: doesn't check if return statement if valid (eg outside of function) - resolver should do that in next phase
            //Used for type checking for function and return
            Token m_return_type = Token(TokenType::NIL_TYPE, "init", 0);
            bool m_had_return = false;
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
                if (match(TokenType::PRINT))            return print_statement();
                if (match(TokenType::IF))               return if_statement();
                if (peek_one(TokenType::LEFT_BRACE))    return block_statement();
                if (peek_one(TokenType::IDENTIFIER))    return identifier_statement();
                if (match(TokenType::WHILE))            return while_statement();
                if (match(TokenType::FOR))              return for_statement();
                if (match(TokenType::RETURN))           return return_statement();
                
                throw ParseError(previous(), "Invalid token");
            }


            std::shared_ptr<Stmt> return_statement() {
                m_had_return = true;
                Token name = previous();
                std::shared_ptr<Expr> value = expression();
                consume(TokenType::SEMICOLON, "Expect ';' after statement.");
                return std::make_shared<Return>(name, m_return_type, value);
            }

            //starts with identifier - declaration, assignment or function call with unused result
            std::shared_ptr<Stmt> identifier_statement() {
                if (peek_three(TokenType::IDENTIFIER, TokenType::COLON_COLON, TokenType::LEFT_PAREN)) {
                    return function_declaration();
                } else { 
                    //variable decl/assignment is a statement, but expression() wraps it inside an expression
                    //so it needs to be unwrapped and only the member statement is returned
                    std::shared_ptr<Expr> expr = expression();
                    std::shared_ptr<Stmt> stmt = dynamic_cast<StmtExpr*>(expr.get())->m_stmt;
                    consume(TokenType::SEMICOLON, "Expect semicolon after statement.");
                    return stmt;
                }
            }

            std::shared_ptr<Stmt> function_declaration() {
                match(TokenType::IDENTIFIER);
                Token identifier = previous();
                match(TokenType::COLON_COLON);

                //parameter list
                consume(TokenType::LEFT_PAREN, "Expect '(' before function parameters.");

                std::vector<std::shared_ptr<Stmt>> parameters;
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
                    Token type = previous();

                    if (type.m_type == TokenType::COLON) {
                        throw ParseError(type, "Invalid parameter type.");
                    }

                    parameters.emplace_back(std::make_shared<VarDecl>(name, type, nullptr));
                    match(TokenType::COMMA);                    
                }                

                consume(TokenType::RIGHT_ARROW, "Expect '->' and return type after parameter declaration.");

                //TODO: this is ugly and error-prone - find a nicer way to do this
                match(TokenType::BOOL_TYPE);
                match(TokenType::INT_TYPE);
                match(TokenType::FLOAT_TYPE);
                match(TokenType::STRING_TYPE);
                match(TokenType::FUN_TYPE);
                match(TokenType::NIL_TYPE);
                m_return_type = previous(); 
                m_had_return = false;

                consume(TokenType::LEFT_BRACE, "Expect '{' to start new block.");
                std::vector<std::shared_ptr<Stmt>> statements;
                while (!match(TokenType::RIGHT_BRACE)) {
                    statements.push_back(statement());
                }

                if (m_return_type.m_type != TokenType::NIL_TYPE && !m_had_return) {
                    throw ParseError(m_return_type, "Expect return statement of type " + m_return_type.to_string());
                }

                std::shared_ptr<Stmt> body = std::make_shared<Block>(statements);

                return std::make_shared<FunDecl>(identifier, parameters, m_return_type, body);
            }

            std::shared_ptr<Stmt> print_statement() {
                Token name = previous();
                std::shared_ptr<Expr> value = expression(); //this needs to go through recursive descent
                std::shared_ptr<Stmt> ret = std::make_shared<Print>(name, value);
                consume(TokenType::SEMICOLON, "Expect semicolon after statement.");
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



            std::shared_ptr<Stmt> while_statement() {
                Token name = previous();
                consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
                std::shared_ptr<Expr> condition = expression();
                consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
                std::shared_ptr<Stmt> body = block_statement();
                return std::make_shared<While>(name, condition, body);
            }

            std::shared_ptr<Stmt> for_statement() {
                Token name = previous();
                consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
                std::shared_ptr<Stmt> initializer;
                if(!match(TokenType::SEMICOLON)) {
                    initializer = statement(); //statement consumes ';'
                }
                std::shared_ptr<Expr> condition;
                if(!match(TokenType::SEMICOLON)) {
                    condition = expression();
                    consume(TokenType::SEMICOLON, "Expect semicolon after condition.");
                }
                std::shared_ptr<Expr> update;
                if(!match(TokenType::RIGHT_PAREN)) {
                    update = expression();
                    consume(TokenType::RIGHT_PAREN, "Expect ')' after for loop initialization.");
                }
                std::shared_ptr<Stmt> body = block_statement();
                return std::make_shared<For>(name, initializer, condition, update, body);
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
                    std::shared_ptr<Expr> value = declare_assign();
                    std::shared_ptr<Stmt> assignment = std::make_shared<Assign>(identifier, value);
                    return std::make_shared<StmtExpr>(assignment);
                //variable declaration
                } else if (peek_two(TokenType::IDENTIFIER, TokenType::COLON)) {
                    match(TokenType::IDENTIFIER);
                    Token identifier = previous();
                    match(TokenType::COLON);
                    
                    //TODO: this is ugly and error-prone - find a nicer way to do this
                    match(TokenType::BOOL_TYPE);
                    match(TokenType::INT_TYPE);
                    match(TokenType::FLOAT_TYPE);
                    match(TokenType::STRING_TYPE);
                    match(TokenType::FUN_TYPE);
                    Token type = previous();

                    if (type.m_type == TokenType::COLON) {
                        throw ParseError(type, "Invalid data type.");
                    }
                    
                    //check for possible assignment
                    std::shared_ptr<Expr> value = nullptr;
                    if(match(TokenType::EQUAL)) {
                        value = expression();
                    }

                    std::shared_ptr<Stmt> var_decl = std::make_shared<VarDecl>(identifier, type, value);

                    return std::make_shared<StmtExpr>(var_decl);
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
                }else if(match(TokenType::IDENTIFIER)) {
                    if (peek_one(TokenType::LEFT_PAREN)) {
                        Token identifier = previous();
                        consume(TokenType::LEFT_PAREN, "Expected '(' after function identifier");
                        std::vector<std::shared_ptr<Expr>> arguments;
                        while(!match(TokenType::RIGHT_PAREN)) {
                            arguments.emplace_back(expression());
                            match(TokenType::COMMA);
                        }
                        return std::make_shared<StmtExpr>(std::make_shared<Call>(identifier, arguments));
                    } else {
                        return std::make_shared<Variable>(previous());
                    }
                }else if(match(TokenType::LEFT_PAREN)) {
                    Token t = previous();
                    std::shared_ptr<Expr> expr = expression();
                    consume(TokenType::RIGHT_PAREN, "Expect closing parenthesis");
                    return std::make_shared<Group>(t, expr);
                }

                std::cout << m_tokens.at(m_current).to_string() << std::endl;
                throw ParseError(previous(), "Expecting an expression.");
            }


            bool match(TokenType type) {
                if(m_tokens.at(m_current).m_type == type) {
                    m_current++;
                    return true;
                }
                return false;
            }

            bool peek_one(TokenType type) {
                return m_tokens.at(m_current).m_type == type;
            }
            bool peek_two(TokenType type1, TokenType type2) {
                return peek_one(type1) && (m_tokens.at(m_current + 1).m_type == type2);
            }

            bool peek_three(TokenType type1, TokenType type2, TokenType type3) {
                return peek_two(type1, type2) && (m_tokens.at(m_current + 2).m_type == type3);
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
