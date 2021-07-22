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
            Token(TokenType type, const std::string& lexeme, int line);
            ~Token() {}
            std::string to_string();
    };

}

#endif //ZEBRA_TOKEN_H
