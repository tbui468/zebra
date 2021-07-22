#ifndef ZEBRA_TOKEN_ARRAY_H
#define ZEBRA_TOKEN_ARRAY_H

#include "Token.hpp"

namespace zebra {

    class TokenArray {
        private:
            Token* m_tokens;
            const int DEFAULT_SIZE = 8;
            int m_capacity = DEFAULT_SIZE;
            int m_size = 0;
        public:
            TokenArray();
            ~TokenArray();
            const Token& at(int index) const;
            int size() const;
            void add(const Token& token);
//            void print();
        private:
            void init();
            void double_size();
    };

}


#endif //ZEBRA_TOKEN_ARRAY_H
