#include <string.h>
#include <stdio.h>
#include "Token.hpp"


namespace zebra {

    Token::Token(TokenType type, const char* start, int len, int line): m_type(type), m_line(line) {
        if(start) strncpy_s(m_lexeme, Token::MAX_LEXEME_SIZE, start, len);
    }
    void Token::print() {
        switch(m_type) {
            //single char tokens
            case TokenType::PLUS: printf("PLUS"); break;
            case TokenType::MINUS: printf("MINUS"); break;
            case TokenType::STAR: printf("STAR"); break;
            case TokenType::SLASH: printf("SLASH"); break;
            case TokenType::DOT: printf("DOT"); break;
            case TokenType::SEMICOLON: printf("SEMICOLON"); break;
            case TokenType::LEFT_PAREN: printf("LEFT_PAREN"); break;
            case TokenType::RIGHT_PAREN: printf("RIGHT_PAREN"); break;
            case TokenType::LEFT_BRACE: printf("LEFT_BRACE"); break;
            case TokenType::RIGHT_BRACE: printf("RIGHT_BRACE"); break;
            case TokenType::MOD: printf("MOD"); break;
            //double or single char tokens
            case TokenType::EQUAL: printf("EQUAL"); break;
            case TokenType::EQUAL_EQUAL: printf("EQUAL_EQUAL"); break;
            case TokenType::LESS: printf("LESS"); break;
            case TokenType::LESS_EQUAL: printf("LESS_EQUAL"); break;
            case TokenType::GREATER: printf("GREATER"); break;
            case TokenType::GREATER_EQUAL: printf("GREATER_EQUAL"); break;
            case TokenType::BANG: printf("BANG"); break;
            case TokenType::BANG_EQUAL: printf("BANG_EQUAL"); break;
            //literals
            case TokenType::INT: printf("INT [%s]", m_lexeme); break;
            case TokenType::FLOAT: printf("FLOAT [%s]", m_lexeme); break;
            case TokenType::STRING: printf("STRING [%s]", m_lexeme); break;
            case TokenType::IDENTIFIER: printf("IDENTIFIER [%s]", m_lexeme); break;
            //keywords
            case TokenType::PRINT: printf("PRINT"); break;
            case TokenType::IF: printf("IF"); break;
            case TokenType::ELSE: printf("ELSE"); break;
            case TokenType::TRUE: printf("TRUE"); break;
            case TokenType::FALSE: printf("FALSE"); break;
            //other
            case TokenType::EOFILE: printf("EOFILE"); break;
        }
        printf("\n");
    }

}
