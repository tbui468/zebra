#ifndef ZEBRA_LEXER_H
#define ZEBRA_LEXER_H

#include <string>
#include <vector>
#include <iostream>

#include "Token.hpp"

namespace zebra {

    class Lexer {
        private:
            std::string m_source = "";
            std::vector<Token> m_tokens;
            int m_current = 0;
            int m_line = 1;
            class SyntaxError {
                private:
                    int m_line;
                    std::string m_message;
                public:
                    SyntaxError(int line, const std::string& message): m_line(line), m_message(message){}
                    ~SyntaxError() {}
                    void print() {
                        std::cout << "[Line " << m_line << "] Syntax Error: " << m_message << std::endl;
                    }
            };
        public:
            Lexer(const char* file_path);
            ~Lexer();
            std::vector<Token> scan();
            void print_source() const;
        private:
            void advance_to_next_line();
            void scan_source();
            void read_string();
            void read_identifier();
            bool match(const std::string& s);
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
            void add_token(TokenType type, const std::string& lexeme);
            void read_source(const char* file_path);

    };
}


#endif //ZEBRA_LEXER_H
