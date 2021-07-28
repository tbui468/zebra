#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>

namespace zebra {

    struct Stmt;
    struct StructDecl;

    class Object {
        public:
            Object() {}
            virtual ~Object() {}
            virtual std::shared_ptr<Object> clone() = 0;
    };

    class Bool: public Object {
        public:
            bool m_value;
        public:
            Bool(bool value): m_value(value) {}
            Bool(const Bool& obj): m_value(obj.m_value) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<Bool>(*this);
            }
    };

    class Int: public Object {
        public:
            int m_value;
        public:
            Int(int value): m_value(value) {}
            Int(const Int& obj): m_value(obj.m_value) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<Int>(*this);
            }
    };

    class Float: public Object {
        public:
            float m_value;
        public:
            Float(float value): m_value(value) {}
            Float(const Float& obj): m_value(obj.m_value) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<Float>(*this);
            }
    };
    class String: public Object {
        public:
            std::string m_value;
        public:
            String(std::string value): m_value(value) {}
            String(const String& obj): m_value(obj.m_value) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<String>(*this);
            }
    };

    class Nil: public Object {
        public:
        public:
            Nil() {}
            Nil(const Nil& obj) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<Nil>(*this);
            }
    };
    
    class Fun: public Object {
        public:
            Stmt* m_fun_decl;
        public:
            Fun(Stmt* fun_decl): m_fun_decl(fun_decl) {}
            Fun(const Fun& obj): m_fun_decl(obj.m_fun_decl) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<Fun>(*this);
            }
    };

    class StructDefinition: public Object {
        public:
            StructDecl* m_node;
        public:
            StructDefinition(StructDecl* node): m_node(node) {}
            StructDefinition(const StructDefinition& obj): m_node(obj.m_node) {}
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<StructDefinition>(*this);
            }
    };

    class StructInstance: public Object {
        public:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_fields;
        public:
            StructInstance(std::unordered_map<std::string, std::shared_ptr<Object>> fields): m_fields(fields) {}
            StructInstance(const StructInstance& obj) {
                for(std::pair<std::string, std::shared_ptr<Object>> it: obj.m_fields) {
                    m_fields[it.first] = it.second->clone();
                }
            }
            virtual std::shared_ptr<Object> clone() override {
                return std::make_shared<StructInstance>(*this);
            }
            
    };

}


#endif //ZEBRA_OBJECT_H
