#ifndef ZEBRA_LEXER_H
#define ZEBRA_LEXER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "Token.hpp"

namespace zebra {

    enum class ResultCode {
        SUCCESS,
        FAILED
    };


    struct SyntaxError {
        int m_line;
        std::string m_message;
        SyntaxError(int line, const std::string& message): m_line(line), m_message(message) {}
    };

    class Lexer {
        private:
            std::string m_source = "";
            int m_current = 0;
            int m_line = 1;
            std::vector<SyntaxError> m_errors;

        public:
            Lexer(const char* file_path) {
                read_source(file_path);
            }

            ~Lexer() {}

            ResultCode scan(std::vector<Token>& tokens) {
                m_errors.clear();
                scan_source(tokens);

                if (m_errors.empty())
                    return ResultCode::SUCCESS;
                else
                    return ResultCode::FAILED;
            }

            void print_source() const {
                std::cout << m_source;
            }

            std::vector<SyntaxError> get_errors() {
                return m_errors;
            }
        private:
            void scan_source(std::vector<Token>& tokens) {
                while (!is_at_end()) {
                    char c = next();
                    switch(c) {
                        //single char tokens
                        case '+': add_token(tokens, TokenType::PLUS); break;
                        case '-': 
                            if(match('>')) add_token(tokens, TokenType::RIGHT_ARROW);
                            else add_token(tokens, TokenType::MINUS); 
                            break;
                        case '/': 
                            if(match('/'))  advance_to_next_line();
                            else            add_token(tokens, TokenType::SLASH);
                            break;
                        case '*': add_token(tokens, TokenType::STAR); break;
                        case ';': add_token(tokens, TokenType::SEMICOLON); break;
                        case '(': add_token(tokens, TokenType::LEFT_PAREN); break;
                        case ')': add_token(tokens, TokenType::RIGHT_PAREN); break;
                        case '{': add_token(tokens, TokenType::LEFT_BRACE); break;
                        case '}': add_token(tokens, TokenType::RIGHT_BRACE); break;
                        case '%': add_token(tokens, TokenType::MOD); break;
                        case ',': add_token(tokens, TokenType::COMMA); break;
                        case ':': 
                            if (match(':')) add_token(tokens, TokenType::COLON_COLON);
                            else            add_token(tokens, TokenType::COLON);
                            break;
                        case '.': 
                            if (is_numeric(peek())) {
                                int start = m_current - 1;
                                while(!is_at_end() && is_numeric(peek())) {
                                    next();
                                }
                                int len = m_current - start;
                                add_token(tokens, TokenType::FLOAT, m_source.substr(start, len));
                            } else {                     
                                add_token(tokens, TokenType::DOT); 
                            }
                            break;
                        //double or single char tokens
                        case '=':
                            if (match('=')) add_token(tokens, TokenType::EQUAL_EQUAL);
                            else            add_token(tokens, TokenType::EQUAL);
                            break;
                        case '<':
                            if (match('=')) add_token(tokens, TokenType::LESS_EQUAL);
                            else            add_token(tokens, TokenType::LESS);
                            break;
                        case '>':
                            if (match('=')) add_token(tokens, TokenType::GREATER_EQUAL);
                            else            add_token(tokens, TokenType::GREATER);
                            break;
                        case '!':
                            if (match('=')) add_token(tokens, TokenType::BANG_EQUAL);
                            else            add_token(tokens, TokenType::BANG);
                            break;
                        //literals, keywords, and eofile
                        case '"': 
                            add_token(tokens, TokenType::STRING, read_string());
                            break;
                        case '\n': m_line++;
                        case '\r':
                        case ' ':
                            break;
                        case 'a':
                            if (match("nd")) add_token(tokens, TokenType::AND);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'b':
                            if (match("ool")) add_token(tokens, TokenType::BOOL_TYPE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'e':
                            if (match("lse")) add_token(tokens, TokenType::ELSE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'f':
                            if (match("alse")) add_token(tokens, TokenType::FALSE);
                            else if(match("loat")) add_token(tokens, TokenType::FLOAT_TYPE);
                            else if(match("or")) add_token(tokens, TokenType::FOR);
                            else if(match("un")) add_token(tokens, TokenType::FUN_TYPE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'i':
                            if (match("f")) add_token(tokens, TokenType::IF);
                            else if(match("nt")) add_token(tokens, TokenType::INT_TYPE);
                            else if(match("mport")) add_token(tokens, TokenType::IMPORT);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'o':
                            if (match("r")) add_token(tokens, TokenType::OR);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'n':
                            if (match("il")) add_token(tokens, TokenType::NIL_TYPE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'r':
                            if (match("eturn")) {
                                add_token(tokens, TokenType::RETURN);
                                if (peek() == ';') { //this really should be here - parser should take care of this
                                    add_token(tokens, TokenType::NIL);
                                }
                            }
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 's':
                            if (match("tring")) add_token(tokens, TokenType::STRING_TYPE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 't':
                            if (match("rue")) add_token(tokens, TokenType::TRUE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        case 'w':
                            if (match("hile")) add_token(tokens, TokenType::WHILE);
                            else add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            break;
                        default:
                            if (is_numeric(c)) {
                                int start = m_current - 1;
                                while(!is_at_end() && is_numeric(peek())) {
                                    next();
                                }

                                //integer
                                if (!is_at_end() && peek() != '.') {
                                    int len = m_current - start;
                                    add_token(tokens, TokenType::INT, m_source.substr(start, len));
                                    return;
                                } else {
                                    if (!is_at_end() && peek() == '.') {
                                        next();               
                                        while(!is_at_end() && is_numeric(peek())) {
                                            next();
                                        }
                                    }

                                    int len = m_current - start;
                                    add_token(tokens, TokenType::FLOAT, m_source.substr(start, len));
                                }
                            } else if (is_alpha(c)) {
                                add_token(tokens, TokenType::IDENTIFIER, read_identifier());
                            } else {
                                m_errors.emplace_back(m_line, "Unrecognized character.");
                            }
                            break;
                    }
                }
                add_token(tokens, TokenType::EOFILE);
            }

            void advance_to_next_line() {
                while(!is_at_end() && peek() != '\n') {
                    next();
                }
            }

            void add_token(std::vector<Token>& tokens, TokenType type, const std::string& lexeme) {
                tokens.emplace_back(Token(type, lexeme, m_line));
            }

            void add_token(std::vector<Token>& tokens, TokenType type) {
                tokens.emplace_back(Token(type, "", m_line));
            }

            std::string read_string() {
                int start = m_current; //not including quote
                while(!is_at_end() && peek() != '"') {
                    next();
                }

                if (peek() != '"') m_errors.emplace_back(m_line, "Unclosed double quotes.");

                next();

                int len = m_current - start - 1; //removing two quotes, but adding on extra space for null terminater
                return m_source.substr(start, len);
            }
            

            std::string read_identifier() {
                int start = m_current - 1;
                while(!is_at_end() && is_alpha_numeric(peek())) {
                    next();
                }

                int len = m_current - start;
                return m_source.substr(start, len);
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
