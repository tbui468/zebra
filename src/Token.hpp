#ifndef ZEBRA_TOKEN_H
#define ZEBRA_TOKEN_H

#include <string>
#include "TokenType.hpp"


namespace zebra {

    struct Token {
        public:
            TokenType m_type;
            std::string m_lexeme;
            int m_line;
        public:
            static std::string to_string(TokenType type) {
                switch(type) {
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
                    case TokenType::COMMA: return "COMMA";
                    //double or single char tokens
                    case TokenType::EQUAL: return "EQUAL"; break;
                    case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL"; break;
                    case TokenType::LESS: return "LESS"; break;
                    case TokenType::LESS_EQUAL: return "LESS_EQUAL"; break;
                    case TokenType::GREATER: return "GREATER"; break;
                    case TokenType::GREATER_EQUAL: return "GREATER_EQUAL"; break;
                    case TokenType::BANG: return "BANG"; break;
                    case TokenType::BANG_EQUAL: return "BANG_EQUAL"; break;
                    case TokenType::COLON_COLON: return "COLON_COLON"; break;
                    case TokenType::RIGHT_ARROW: return "RIGHT_ARROW"; break;
                    //literals
                    case TokenType::INT: return "INT"; break;
                    case TokenType::FLOAT: return "FLOAT"; break;
                    case TokenType::STRING: return "STRING"; break;
                    case TokenType::IDENTIFIER: return "IDENTIFIER"; break;
                    case TokenType::NIL: return "NIL";
                    //keywords
                    case TokenType::IF: return "IF"; break;
                    case TokenType::ELSE: return "ELSE"; break;
                    case TokenType::TRUE: return "TRUE"; break;
                    case TokenType::FALSE: return "FALSE"; break;
                    case TokenType::OR: return "OR"; break;
                    case TokenType::AND: return "AND";
                    case TokenType::WHILE: return "WHILE";
                    case TokenType::FOR: return "FOR";
                    case TokenType::RETURN: return "RETURN";
                    case TokenType::CLASS: return "CLASS";
                    case TokenType::IMPORT: return "IMPORT";
                    case TokenType::PRINT: return "PRINT";
                    //types
                    case TokenType::INT_TYPE: return "INT_TYPE";
                    case TokenType::FLOAT_TYPE: return "FLOAT_TYPE";
                    case TokenType::STRING_TYPE: return "STRING_TYPE";
                    case TokenType::BOOL_TYPE: return "BOOL_TYPE";
                    case TokenType::FUN_TYPE: return "FUN_TYPE"; //is this used? Using function signatures (parameter/return types)
                    case TokenType::NIL_TYPE: return "NIL_TYPE";
                    case TokenType::CLASS_TYPE: return "CLASS_TYPE";
                    //other
                    case TokenType::SLASH_SLASH: return "SLASH_SLASH";
                    case TokenType::ERROR: return "ERROR";
                    case TokenType::EOFILE: return "EOFILE";
                    default: return "Invalid token type";
                }
            }
        public:
            Token(): m_type(TokenType::NIL), m_lexeme(""), m_line(-1) {}
            Token(TokenType type): m_type(type), m_lexeme(""), m_line(-1) {}
            Token(TokenType type, const std::string& lexeme): m_type(type), m_lexeme(lexeme), m_line(-1) {}
            Token(TokenType type, const std::string& lexeme, int line): m_type(type), m_lexeme(lexeme), m_line(line) {}
            ~Token() {}

            std::string to_string() {
                std::string token = Token::to_string(m_type);
                if (m_lexeme != "") {
                    return token + " [" + m_lexeme + "]";
                } else {
                    return token;
                }
            }
    };

}

#endif //ZEBRA_TOKEN_H
