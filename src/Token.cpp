#include <string.h>
#include <stdio.h>
#include "Token.hpp"


namespace zebra {

    Token::Token(TokenType type, const std::string& lexeme, int line): m_type(type), m_lexeme(lexeme), m_line(line) {}
    std::string Token::to_string() {
        switch(m_type) {
            //single char tokens
            case TokenType::PLUS: return "PLUS"; break;
            case TokenType::MINUS: return "MINUS"; break;
            case TokenType::STAR: return "STAR"; break;
            case TokenType::SLASH: return "SLASH"; break;
            case TokenType::DOT: return "DOT"; break;
            case TokenType::SEMICOLON: return "SEMICOLON"; break;
            case TokenType::LEFT_PAREN: return "LEFT_PAREN"; break;
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN"; break;
            case TokenType::LEFT_BRACE: return "LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE"; break;
            case TokenType::MOD: return "MOD";
            case TokenType::COLON: return "COLON";
            //double or single char tokens
            case TokenType::EQUAL: return "EQUAL"; break;
            case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL"; break;
            case TokenType::LESS: return "LESS"; break;
            case TokenType::LESS_EQUAL: return "LESS_EQUAL"; break;
            case TokenType::GREATER: return "GREATER"; break;
            case TokenType::GREATER_EQUAL: return "GREATER_EQUAL"; break;
            case TokenType::BANG: return "BANG"; break;
            case TokenType::BANG_EQUAL: return "BANG_EQUAL"; break;
            //literals
            case TokenType::INT: return "INT [" + m_lexeme + "]"; break;
            case TokenType::FLOAT: return "FLOAT [" + m_lexeme + "]"; break;
            case TokenType::STRING: return "STRING [" + m_lexeme + "]"; break;
            case TokenType::IDENTIFIER: return "IDENTIFIER [" + m_lexeme + "]"; break;
            //keywords
            case TokenType::PRINT: return "PRINT"; break;
            case TokenType::IF: return "IF"; break;
            case TokenType::ELSE: return "ELSE"; break;
            case TokenType::TRUE: return "TRUE"; break;
            case TokenType::FALSE: return "FALSE"; break;
            case TokenType::OR: return "OR"; break;
            case TokenType::AND: return "AND";
            case TokenType::INT_TYPE: return "INT_TYPE";
            case TokenType::FLOAT_TYPE: return "FLOAT_TYPE";
            case TokenType::STRING_TYPE: return "STRING_TYPE";
            case TokenType::BOOL_TYPE: return "BOOL_TYPE";
            case TokenType::WHILE: return "WHILE";
            //other
            case TokenType::SLASH_SLASH: return "SLASH_SLASH";
            case TokenType::EOFILE: return "EOFILE";
        }
    }

}
