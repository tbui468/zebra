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

    FunDef::FunDef(std::vector<std::shared_ptr<Expr>> parameters, std::shared_ptr<Expr> body)
        : m_parameters(parameters), m_body(body) {}

    FunDef::FunDef(const FunDef& obj): m_parameters(obj.m_parameters), m_body(obj.m_body) {}

    std::shared_ptr<Object> FunDef::clone() {
        return std::make_shared<FunDef>(*this);
    }

    std::shared_ptr<Object> FunDef::call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) {

        for (int i = 0; i < m_parameters.size(); i++) {
            Token param_token = dynamic_cast<DeclVar*>(m_parameters.at(i).get())->m_name;
            std::shared_ptr<Object> param_value = arguments.at(i);
            interp->m_environment->define(param_token, param_value);
        }

        interp->evaluate(m_body.get()); //this should set env return value, which we grab and return

        return interp->m_environment->get_return();
    }


    ClassDef::ClassDef(std::shared_ptr<Object> base,
                       std::vector<std::pair<Token, std::shared_ptr<Object>>> fields, 
                       std::vector<std::pair<Token, std::shared_ptr<Object>>> methods):
                            m_base(base), m_fields(fields), m_methods(methods) {}
    ClassDef::ClassDef(const ClassDef& obj) {
        //TODO: this should never happen, right?
    }
    std::shared_ptr<Object> ClassDef::clone() {
        return std::make_shared<ClassDef>(*this);
    }
            
    ClassInst::ClassInst(std::shared_ptr<Environment> global_env, std::shared_ptr<ClassDef> def): m_class(def) {
        m_environment = std::make_shared<Environment>(global_env, false);
        for (std::pair<Token, std::shared_ptr<Object>> p: def->m_fields) {
            m_environment->define(p.first, p.second);
        }
        for (std::pair<Token, std::shared_ptr<Object>> p: def->m_methods) {
            m_environment->define(p.first, p.second);
        }
    }
    ClassInst::ClassInst(const ClassInst& obj) {
        //TODO: this should never happen, right?  Otherwise we need to clone the environment
    }
    std::shared_ptr<Object> ClassInst::clone() {
        return std::make_shared<ClassInst>(*this);
    }

}
