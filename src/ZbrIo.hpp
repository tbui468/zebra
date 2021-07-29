#ifndef ZEBRA_IO_H
#define ZEBRA_IO_H


#include "Interpreter.hpp"
#include "Object.hpp"

namespace zebra {

    class Print: public Callable {
        public:
            Print() = default;
            virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) override {
                std::shared_ptr<Object> value = arguments.at(0);

                if(dynamic_cast<Bool*>(value.get())) {
                    std::cout << dynamic_cast<Bool*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<Int*>(value.get())) {
                    std::cout << dynamic_cast<Int*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<Float*>(value.get())) {
                    std::cout << dynamic_cast<Float*>(value.get())->m_value << std::endl;
                }
                if(dynamic_cast<String*>(value.get())) {
                    std::cout << dynamic_cast<String*>(value.get())->m_value << std::endl;
                }

                return std::make_shared<Nil>();
            }
            std::shared_ptr<Object> clone() override {
                return std::make_shared<Print>(*this);
            }
    };

}

#endif // ZEBRA_IO_H
