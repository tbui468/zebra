#ifndef ZEBRA_ENVIRONMENT_H
#define ZEBRA_ENVIRONMENT_H

#include <memory>
#include <unordered_map>
#include "RuntimeError.hpp"
#include "Token.hpp"


namespace zebra {

    class Object;

    class Environment {
        private:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_values;
            std::shared_ptr<Environment> m_closure {nullptr};
            std::shared_ptr<Object> m_return {nullptr};
            bool m_is_function {false};
        public:
            Environment(std::shared_ptr<Environment> closure, bool is_func);
            Environment();
            void define(Token name, std::shared_ptr<Object> value);
            void assign(Token name, std::shared_ptr<Object> value);
            std::shared_ptr<Object> get(Token name);
            void set_return(std::shared_ptr<Object> ret);
            std::shared_ptr<Object> get_return();
    };


}





#endif // ZEBRA_ENVIRONMENT_H

