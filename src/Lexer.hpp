#ifndef ZEBRA_LEXER_H
#define ZEBRA_LEXER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

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
            Lexer(const char* file_path) {
                read_source(file_path);
            }

            ~Lexer() {}

            std::vector<Token> scan() {
                try {
                    scan_source();
                }catch(SyntaxError& e) {
                    e.print();
                }
                return m_tokens;
            }

            void print_source() const {
                std::cout << m_source;
            }
        private:
            void scan_source() {
                while (!is_at_end()) {
                    char c = next();
                    switch(c) {
                        //single char tokens
                        case '+': add_token(TokenType::PLUS); break;
                        case '-': 
                            if(match('>')) add_token(TokenType::RIGHT_ARROW);
                            else add_token(TokenType::MINUS); 
                            break;
                        case '/': 
                            if(match('/'))  advance_to_next_line();
                            else            add_token(TokenType::SLASH);
                            break;
                        case '*': add_token(TokenType::STAR); break;
                        case ';': add_token(TokenType::SEMICOLON); break;
                        case '(': add_token(TokenType::LEFT_PAREN); break;
                        case ')': add_token(TokenType::RIGHT_PAREN); break;
                        case '{': add_token(TokenType::LEFT_BRACE); break;
                        case '}': add_token(TokenType::RIGHT_BRACE); break;
                        case '%': add_token(TokenType::MOD); break;
                        case ',': add_token(TokenType::COMMA); break;
                        case ':': 
                            if (match(':')) add_token(TokenType::COLON_COLON);
                            else add_token(TokenType::COLON);
                            break;
                        case '.': 
                            if(is_numeric(peek()))   read_float_with_leading();
                            else                     add_token(TokenType::DOT); 
                            break;
                        //double or single char tokens
                        case '=':
                            if (match('=')) add_token(TokenType::EQUAL_EQUAL);
                            else            add_token(TokenType::EQUAL);
                            break;
                        case '<':
                            if (match('=')) add_token(TokenType::LESS_EQUAL);
                            else            add_token(TokenType::LESS);
                            break;
                        case '>':
                            if (match('=')) add_token(TokenType::GREATER_EQUAL);
                            else            add_token(TokenType::GREATER);
                            break;
                        case '!':
                            if (match('=')) add_token(TokenType::BANG_EQUAL);
                            else            add_token(TokenType::BANG);
                            break;
                        //literals, keywords, and eofile
                        case '"': read_string(); break;
                        case '\n': m_line++;
                        case '\r':
                        case ' ':
                            break;
                        case 'a':
                            if (match("nd")) add_token(TokenType::AND);
                            else read_identifier();
                            break;
                        case 'b':
                            if (match("ool")) add_token(TokenType::BOOL_TYPE);
                            else read_identifier();
                            break;
                        case 'e':
                            if (match("lse")) add_token(TokenType::ELSE);
                            else read_identifier();
                            break;
                        case 'f':
                            if (match("alse")) add_token(TokenType::FALSE);
                            else if(match("loat")) add_token(TokenType::FLOAT_TYPE);
                            else if(match("or")) add_token(TokenType::FOR);
                            else if(match("un")) add_token(TokenType::FUN_TYPE);
                            else read_identifier();
                            break;
                        case 'i':
                            if (match("f")) add_token(TokenType::IF);
                            else if(match("nt")) add_token(TokenType::INT_TYPE);
                            else read_identifier();
                            break;
                        case 'o':
                            if (match("r")) add_token(TokenType::OR);
                            else read_identifier();
                            break;
                        case 'n':
                            if (match("il")) add_token(TokenType::NIL_TYPE);
                            else read_identifier();
                            break;
                        case 'p':
                            if (match("rint")) add_token(TokenType::PRINT);
                            else read_identifier();
                            break;
                        case 'r':
                            if (match("eturn")) {
                                add_token(TokenType::RETURN);
                                if (peek() == ';') { //this really should be here - parser should take care of this
                                    add_token(TokenType::NIL);
                                }
                            }
                            else read_identifier();
                            break;
                        case 's':
                            if (match("tring")) add_token(TokenType::STRING_TYPE);
                            else if (match("truct")) add_token(TokenType::STRUCT);
                            else read_identifier();
                            break;
                        case 't':
                            if (match("rue")) add_token(TokenType::TRUE);
                            else read_identifier();
                            break;
                        case 'w':
                            if (match("hile")) add_token(TokenType::WHILE);
                            else read_identifier();
                            break;
                        default:
                            if (is_numeric(c)) {
                                read_number();
                            } else if (is_alpha(c)) {
                                read_identifier();
                            } else {
                                throw SyntaxError(m_line, "Invalid character.\n");
                            }
                            break;
                    }
                }
                add_token(TokenType::EOFILE);
            }

            void advance_to_next_line() {
                while(!is_at_end() && peek() != '\n') {
                    next();
                }
            }


            void read_string() {
                int start = m_current; //not including quote
                while(!is_at_end() && peek() != '"') {
                    next();
                }

                if (peek() != '"') throw SyntaxError(m_line, "Unclosed double quotes.\n");

                next();

                int len = m_current - start - 1; //removing two quotes, but adding on extra space for null terminater
                add_token(TokenType::STRING, m_source.substr(start, len));
            }
            

            void read_identifier() {
                int start = m_current - 1;
                while(!is_at_end() && is_alpha(peek())) {
                    next();
                }

                int len = m_current - start;
                add_token(TokenType::IDENTIFIER, m_source.substr(start, len));
            }

            bool match(const std::string& s) {

                if (s != m_source.substr(m_current, s.length())) {
                    return false;
                }

                //check that next char is also not a letter
                if (is_alpha(char(m_source.at(m_current + s.length())))) {
                    return false;
                }

                for (int i = 0; i < (int)(s.length()); i++) next();

                return true;
            }

            void read_float_with_leading() {
                int start = m_current - 1;
                while(!is_at_end() && is_numeric(peek())) {
                    next();
                }
                int len = m_current - start;
                add_token(TokenType::FLOAT, m_source.substr(start, len));
            }

            void read_number() {
                int start = m_current - 1;
                while(!is_at_end() && is_numeric(peek())) {
                    next();
                }

                //integer
                if (!is_at_end() && peek() != '.') {
                    int len = m_current - start;
                    add_token(TokenType::INT, m_source.substr(start, len));
                    return;
                }

                //allows trailing decimals - find a way to get rid of this
                if (!is_at_end() && peek() == '.') {
                    next();               
                    while(!is_at_end() && is_numeric(peek())) {
                        next();
                    }
                }

                int len = m_current - start;
                add_token(TokenType::FLOAT, m_source.substr(start, len));
            }

            bool match(char c) {
                if(peek() == c) {
                    next();
                    return true;
                }
                return false;
            }

            char next() {
                m_current++;
                return m_source[m_current - 1];
            }

            char peek() {
                assert(m_current < int(m_source.length()));
                return m_source[m_current];
            }

            bool is_alpha(char c) {
                return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
            }

            bool is_alpha_numeric(char c) {
                return (is_alpha(c) || is_numeric(c));
            }

            bool is_at_end() {
                return m_current >= int(m_source.length() - 1);
            }

            bool is_numeric(char c) {
                return c >= '0' && c <= '9';
            }

            void add_token(TokenType type) {
                m_tokens.emplace_back(Token(type, "", m_line));
            }
            void add_token(TokenType type, const std::string& lexeme) {
                m_tokens.emplace_back(Token(type, lexeme, m_line));
            }


            void read_source(const char* file_path) {
                std::string line;
                std::ifstream my_file(file_path);

                if(my_file.is_open()) {
                    while (getline(my_file, line)) {
                      m_source += line + "\n";
                    }
                    my_file.close();
                }

            }

    };
}


#endif //ZEBRA_LEXER_H
