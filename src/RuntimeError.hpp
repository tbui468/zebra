#ifndef ZEBRA_RUNTIME_ERROR_H
#define ZEBRA_RUNTIME_ERROR_H

namespace zebra {

    class RuntimeError {
        private:
            Token m_token;
            std::string m_message;
        public:
            RuntimeError(Token token, const std::string& message): m_token(token), m_message(message){}
            ~RuntimeError() {}
            void print() {
                std::cout << "[Line " << m_token.m_line << "] Error: " << m_message << std::endl;
            }
    };

}


#endif // ZEBRA_RUNTIME_ERROR_H
