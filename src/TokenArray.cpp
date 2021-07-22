#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "TokenArray.hpp"

namespace zebra {
    TokenArray::TokenArray() {
        init();
    }
    TokenArray::~TokenArray() {
        delete[] m_tokens;
    }
    const Token& TokenArray::at(int index) const {
        assert(index < m_size);
        return m_tokens[index];
    }
    int TokenArray::size() const {
        return m_size;
    }
    void TokenArray::add(const Token& token) {
        if (m_size == m_capacity) {
            double_size();
        }
        m_tokens[m_size] = token;
        m_size++;
    }
/*
    void TokenArray::print() {
        for (int i = 0; i < m_size; i++) {
            m_tokens[i].print();
            printf("\n");
        } 
    }*/

    void TokenArray::init() {
        m_tokens = new Token[m_capacity];
    }

    void TokenArray::double_size() {
        m_capacity *= 2;
        Token* new_array = new Token[m_capacity];
        memcpy((void*)new_array, (const void*)m_tokens, sizeof(Token) * m_size);
        delete[] m_tokens;
        m_tokens = new_array;
    }

}
