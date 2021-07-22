#ifndef ZEBRA_TOKEN_H
#define ZEBRA_TOKEN_H

#include "TokenType.hpp"


namespace zebra {

    struct Token {
        private:
            static const int MAX_LEXEME_SIZE = 256;
        public:
            TokenType m_type;
            //int m_length;
            char m_lexeme[MAX_LEXEME_SIZE] = {0};
            int m_line;
        public:
            Token(): Token(TokenType::STRING, nullptr, 0, 0) {}
            Token(TokenType type, const char* start, int len, int line);
            ~Token() {}
            void print();
    };

}

#endif //ZEBRA_TOKEN_H
