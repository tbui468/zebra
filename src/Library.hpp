#ifndef ZEBRA_LIBRARY_H
#define ZEBRA_LIBRARY_H

#include <chrono>
#include "Interpreter.hpp"
#include "Object.hpp"

namespace zebra {

    class Print: public Callable {
        public:
            Print(): Callable({DataType(TokenType::STRING_TYPE), DataType(TokenType::NIL_TYPE)}) {}
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

    class Input: public Callable {
        public:
            Input(): Callable({DataType(TokenType::STRING_TYPE)}) {}
            virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) override {
                std::string line;
                std::getline(std::cin, line);

                return std::make_shared<String>(line);
            }
            std::shared_ptr<Object> clone() override {
                return std::make_shared<Input>(*this);
            }
    };


    class Clock: public Callable {
        public:
            Clock(): Callable({DataType(TokenType::FLOAT_TYPE)}) {}
            virtual std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> arguments, Interpreter* interp) override {
                std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
                double seconds = std::chrono::duration<double>(time.time_since_epoch()).count();
                return std::make_shared<Float>(float(seconds));
            }
            std::shared_ptr<Object> clone() override {
                return std::make_shared<Clock>(*this);
            }
    };

}


#endif// ZEBRA_LIBRARY_H
