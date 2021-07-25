#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>
//#include "Stmt.hpp"

namespace zebra {

    class Object {
        public:
            Object() {}
            virtual ~Object() {}
    };

    class Bool: public Object {
        public:
            bool m_value;
        public:
            Bool(bool value): m_value(value) {}
    };
    class Int: public Object {
        public:
            int m_value;
        public:
            Int(int value): m_value(value) {}
    };
    class Float: public Object {
        public:
            float m_value;
        public:
            Float(float value): m_value(value) {}
    };
    class String: public Object {
        public:
            std::string m_value;
        public:
            String(std::string value): m_value(value) {}
    };

    /*
    class Fun: public Object {
        public:
            std::vector<TokenType> m_arguments;
            TokenType m_return;
            std::shared_ptr<Stmt> m_body;
        public:
            Fun(std::vector<TokenType> arguments, TokenType ret, std::shared_ptr<Stmt> body): 
                m_arguments(arguments), m_return(ret), m_body(body) {}
    };*/

}


#endif //ZEBRA_OBJECT_H
