#ifndef ZEBRA_LEXER_H
#define ZEBRA_LEXER_H

#include <stdio.h>
#include "TokenArray.hpp"

namespace zebra {

    class Lexer {
        private:
            static const int MAX_LEXEME_SIZE = 256;
            char* m_source;
            TokenArray m_tokens;
            int m_length;
            int m_current = 0;
            int m_line = 1;
            class SyntaxError {
                private:
                    int m_line;
                    const char* m_message;
                public:
                    SyntaxError(int line, const char* message): m_line(line), m_message(message){}
                    ~SyntaxError() {}
                    void print() {
                        printf("[Line %d] Syntax Error: %s", m_line, m_message);
                    }
            };
        public:
            Lexer(const char* file_path);
            ~Lexer();
            TokenArray* scan();
        private:
            void scan_source();
            void read_string();
            void read_identifier();
            bool match(const char* s);
            void read_float_with_leading();
            void read_number();
            bool match(char c);
            char next();
            char peek();
            bool is_alpha(char c);
            bool is_alpha_numeric(char c);
            bool is_at_end();
            bool is_numeric(char c);
            void add_token(TokenType type);
            void add_token(TokenType type, const char* start, int len);
            void print_source() const;
            void read_source(const char* file_path);

    };
}


#endif //ZEBRA_LEXER_H
