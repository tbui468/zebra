#ifndef ZEBRA_ENVIRONMENT_H
#define ZEBRA_ENVIRONMENT_H

#include <memory>
#include <unordered_map>
#include "RuntimeError.hpp"


namespace zebra {

    class Environment {
        private:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_values;
            std::shared_ptr<Environment> m_closure;
        public:
            Environment(std::shared_ptr<Environment> closure): m_closure(closure) {}
            Environment(): m_closure(nullptr) {}
            void define(Token name, std::shared_ptr<Object> value) {
                m_values[name.m_lexeme] = value; 
            }
            void assign(Token name, std::shared_ptr<Object> value) {
                if(m_values.count(name.m_lexeme) == 0) {
                    m_closure->assign(name, value);
                }
                m_values[name.m_lexeme] = value; 
            }
            std::shared_ptr<Object> get(Token name) {
                if(m_values.count(name.m_lexeme) == 0) {
                    return m_closure->get(name);
                }

                return m_values[name.m_lexeme];
            }
    };


}





#endif // ZEBRA_ENVIRONMENT_H

