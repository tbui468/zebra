#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>

namespace zebra {

    struct Stmt;

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

    class Nil: public Object {
        public:
        public:
            Nil() {}
    };
    
    class Fun: public Object {
        public:
            Stmt* m_fun_decl;
        public:
            Fun(Stmt* fun_decl): m_fun_decl(fun_decl) {}
    };

    class Struct: public Object {
        public:
            Stmt* m_struct_decl;
        public:
            Struct(Stmt* struct_decl): m_struct_decl(struct_decl) {}
    };

    class StructInstance: public Object {
        public:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_fields;
        public:
            StructInstance(std::unordered_map<std::string, std::shared_ptr<Object>> fields): m_fields(fields) {}
    };

}


#endif //ZEBRA_OBJECT_H
