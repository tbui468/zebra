#ifndef ZEBRA_TIME_H
#define ZEBRA_TIME_H


#include <chrono>
#include "Interpreter.hpp"
#include "Object.hpp"

namespace zebra {

    class Clock: public Callable {
        public:
            Clock(): Callable({Token(TokenType::FLOAT_TYPE)}) {}
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

#endif // ZEBRA_TIME_H
