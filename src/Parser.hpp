#ifndef ZEBRA_PARSER_H
#define ZEBRA_PARSER_H

#include <stdio.h>
#include "Token.hpp"

namespace zebra {


    //Expressions
    struct Expr {

    };

    struct Binary: public Expr {
        public:
        public:
            Token m_op;
            Expr* m_left;
            Expr* m_right;
    };

    struct Unary: public Expr {
        public:
        public:
            Token m_op;
            Expr* m_expr;
    };

    struct Literal: public Expr {
        public:
        public:
            Token m_token;
    };

    //Statements
    struct Stmt {

    };

    struct Print: public Stmt {
        public:
            Print(Expr* value): m_value(value) {}
        public:
            Expr* m_value;
    };


    class Parser {
        private:
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
        Parser(){}
        //statements: print
        //expression -> term -> factor -> unary -> group -> primary
    };

}


#endif //ZEBRA_PARSER_H
