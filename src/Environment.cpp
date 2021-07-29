#include "Environment.hpp"
#include "Object.hpp"

namespace zebra {

    Environment::Environment(std::shared_ptr<Environment> closure, bool is_func): m_closure(closure), m_is_function(is_func) {}

    Environment::Environment() {}

    void Environment::define(Token name, std::shared_ptr<Object> value) {
        m_values[name.m_lexeme] = value; 
    }

    void Environment::assign(Token name, std::shared_ptr<Object> value) {
        if(m_values.count(name.m_lexeme) == 0) {
            m_closure->assign(name, value);
        }
        m_values[name.m_lexeme] = value->clone(); 
    }

    std::shared_ptr<Object> Environment::get(Token name) {
        if(m_values.count(name.m_lexeme) == 0) {
            return m_closure->get(name);
        }

        return m_values[name.m_lexeme];
    }

    void Environment::set_return(std::shared_ptr<Object> ret) {
        if (!m_is_function) {
            m_closure->set_return(ret); 
        } else {
            m_return = ret;
        }
    }

    std::shared_ptr<Object> Environment::get_return() {
        if(m_closure && !m_return) {
            return m_closure->get_return(); 
        }

        return m_return;
    }


}
