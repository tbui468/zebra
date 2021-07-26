#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>

namespace zebra {

    struct Expr;

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

    
    class Fun: public Object {
        public:
            Expr* m_fun_decl;
        public:
            Fun(Expr* fun_decl): m_fun_decl(fun_decl) {}
    };

}


#endif //ZEBRA_OBJECT_H
