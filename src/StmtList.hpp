#ifndef ZEBRA_STMT_LIST_H
#define ZEBRA_STMT_LIST_H

#include <string.h>
#include "Stmt.hpp"


namespace zebra {
    class StmtList {
        private:
            Stmt** m_statements;
            const int DEFAULT_SIZE = 8;
            int m_capacity = DEFAULT_SIZE;
            int m_size = 0;
        public:
            StmtList() {
                init();
                double_size();
            }
            ~StmtList() {
                for (int i = 0; i < m_size; i++) {
                    delete m_statements[i];
                }
                delete[] m_statements;
            }
            Stmt* at(int index) const {
                return m_statements[index];
            }
            int size() const {
                return m_size;
            }
            void add(Stmt* stmt) {
                if(m_size >= m_capacity) {
                    double_size();
                }
                m_statements[m_size] = stmt;
                m_size++;
            }
            void print() {

            }
        private:
            void init() {
                m_statements = new Stmt*[m_capacity];
            }
            void double_size() {
                m_capacity *= 2;
                Stmt** new_array = new Stmt*[m_capacity];
                memcpy((void*)new_array, (const void*)m_statements, sizeof(Stmt*) * m_size);
                delete[] m_statements;
                m_statements = new_array;
            }
    };

}




#endif //ZEBRA_STM_LIST_H
