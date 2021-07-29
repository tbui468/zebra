#include "Object.hpp"

namespace zebra {

    Object::Object() {}
    Object::~Object() {}

    Bool::Bool(bool value): m_value(value) {}
    Bool::Bool(const Bool& obj): m_value(obj.m_value) {}
    std::shared_ptr<Object> Bool::clone() {
        return std::make_shared<Bool>(*this);
    }

    Int::Int(int value): m_value(value) {}
    Int::Int(const Int& obj): m_value(obj.m_value) {}
    std::shared_ptr<Object> Int::clone() {
        return std::make_shared<Int>(*this);
    }

    Float::Float(float value): m_value(value) {}
    Float::Float(const Float& obj): m_value(obj.m_value) {}
    std::shared_ptr<Object> Float::clone() {
        return std::make_shared<Float>(*this);
    }

    String::String(std::string value): m_value(value) {}
    String::String(const String& obj): m_value(obj.m_value) {}
    std::shared_ptr<Object> String::clone() {
        return std::make_shared<String>(*this);
    }

    Nil::Nil() {}
    Nil::Nil(const Nil& obj) {}
    std::shared_ptr<Object> Nil::clone() {
        return std::make_shared<Nil>(*this);
    }

    FunDef::FunDef(std::vector<std::shared_ptr<Stmt>> parameters, std::shared_ptr<Stmt> body)
        : m_parameters(parameters), m_body(body) {}
    FunDef::FunDef(const FunDef& obj): m_parameters(obj.m_parameters), m_body(obj.m_body) {}
    std::shared_ptr<Object> FunDef::clone() {
        return std::make_shared<FunDef>(*this);
    }
    std::shared_ptr<Object> FunDef::call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) {

        for (int i = 0; i < m_parameters.size(); i++) {
            Token param_token = dynamic_cast<VarDecl*>(m_parameters.at(i).get())->m_name;
            std::shared_ptr<Object> param_value = arguments.at(i);
            interp->m_environment->define(param_token, param_value);
        }

        interp->execute(m_body.get()); //this should set env return value, which we grab and return

        return interp->m_environment->get_return();
    }

    StructDef::StructDef(StructDecl* node): m_node(node) {}
    StructDef::StructDef(const StructDef& obj): m_node(obj.m_node) {}
    std::shared_ptr<Object> StructDef::clone() {
        return std::make_shared<StructDef>(*this);
    }

    StructInstance::StructInstance(std::unordered_map<std::string, std::shared_ptr<Object>> fields): m_fields(fields) {}
    StructInstance::StructInstance(const StructInstance& obj) {
        for(std::pair<std::string, std::shared_ptr<Object>> it: obj.m_fields) {
            m_fields[it.first] = it.second->clone();
        }
    }
    std::shared_ptr<Object> StructInstance::clone() {
        return std::make_shared<StructInstance>(*this);
    }
            

}
