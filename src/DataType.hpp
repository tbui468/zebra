#ifndef ZEBRA_DATA_TYPE_H
#define ZEBRA_DATA_TYPE_H

#include <string>
#include "TokenType.hpp"

namespace zebra {

    class DataType {
        public:
            TokenType m_type;
            std::string m_lexeme;
        public:
            DataType(TokenType type, const std::string& lexeme): m_type(type), m_lexeme(lexeme) {}
            DataType(TokenType type): m_type(type), m_lexeme("") {}
            DataType(): m_type(TokenType::NIL_TYPE), m_lexeme("") {}
            ~DataType() {}
            static bool equal(DataType d1, DataType d2) {
                return d1.m_type == d2.m_type && d1.m_lexeme == d2.m_lexeme;
            }
    };

}


#endif // ZEBRA_DATA_TYPE_H
