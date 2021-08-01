#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>
#include <unordered_map>
#include <memory>

#include "Token.hpp"
#include "Interpreter.hpp"

namespace zebra {

    struct Stmt;
    struct StructDecl;
    struct VarDecl;

    class Object {
        public:
            Object();
            virtual ~Object();
            virtual std::shared_ptr<Object> clone() = 0;
    };

    class Bool: public Object {
        public:
            bool m_value;
        public:
            Bool(bool value);
            Bool(const Bool& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class Int: public Object {
        public:
            int m_value;
        public:
            Int(int value);
            Int(const Int& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class Float: public Object {
        public:
            float m_value;
        public:
            Float(float value);
            Float(const Float& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class String: public Object {
        public:
            std::string m_value;
        public:
            String(std::string value);
            String(const String& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class Nil: public Object {
        public:
        public:
            Nil();
            Nil(const Nil& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class Callable: public Object {
        public:
            virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) = 0;
    };
    
    class FunDef: public Callable {
        public:
            std::vector<std::shared_ptr<Expr>> m_parameters;
            std::shared_ptr<Expr> m_body;
        public:
            FunDef(std::vector<std::shared_ptr<Expr>> parameters, std::shared_ptr<Expr> body);
            FunDef(const FunDef& obj);
            virtual std::shared_ptr<Object> clone() override;
            virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) override;
    };

    class StructDef: public Object {
        public:
            StructDecl* m_node;
        public:
            StructDef(StructDecl* node);
            StructDef(const StructDef& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class StructInstance: public Object {
        public:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_fields;
        public:
            StructInstance(std::unordered_map<std::string, std::shared_ptr<Object>> fields);
            StructInstance(const StructInstance& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class ClassDef: public Object {
        public:
            std::vector<std::pair<Token, std::shared_ptr<Object>>> m_fields;
        public:
            ClassDef(std::vector<std::pair<Token, std::shared_ptr<Object>>> fields);
            ClassDef(const ClassDef& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

    class ClassInst: public Object {
        public:
//            std::unordered_map<std::string, std::shared_ptr<Object>> m_fields;
            std::shared_ptr<Environment> m_environment;
        public:
            ClassInst(std::vector<std::pair<Token, std::shared_ptr<Object>>> fields);
            ClassInst(const ClassInst& obj);
            virtual std::shared_ptr<Object> clone() override;
    };

}


#endif //ZEBRA_OBJECT_H
