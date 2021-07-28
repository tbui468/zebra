#ifndef ZEBRA_ENVIRONMENT_H
#define ZEBRA_ENVIRONMENT_H

#include <memory>
#include <unordered_map>
#include "RuntimeError.hpp"
#include "Object.hpp"


namespace zebra {

    class Environment {
        private:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_values;
            std::shared_ptr<Environment> m_closure {nullptr};
            std::shared_ptr<Object> m_return {nullptr};
            bool m_is_function {false};
        public:
            Environment(std::shared_ptr<Environment> closure, bool is_func): m_closure(closure), m_is_function(is_func) {}
            Environment() {}
            void define(Token name, std::shared_ptr<Object> value) {
                m_values[name.m_lexeme] = value; 
            }
            void assign(Token name, std::shared_ptr<Object> value) {
                if(m_values.count(name.m_lexeme) == 0) {
                    m_closure->assign(name, value);
                }
                m_values[name.m_lexeme] = value->clone(); 
            }
            std::shared_ptr<Object> get(Token name) {
                if(m_values.count(name.m_lexeme) == 0) {
                    return m_closure->get(name);
                }

                return m_values[name.m_lexeme];
            }
            void set_return(std::shared_ptr<Object> ret) {
                if (!m_is_function) {
                    m_closure->set_return(ret); 
                } else {
                    m_return = ret;
                }
            }

            std::shared_ptr<Object> get_return() {
                if(m_closure && !m_return) {
                    return m_closure->get_return(); 
                }

                return m_return;

            }
    };


}





#endif // ZEBRA_ENVIRONMENT_H

