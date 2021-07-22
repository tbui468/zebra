#define _CRT_SECURE_NO_WARNINGS //to silence fopen warning
#include <assert.h>
#include <fstream>
#include "Lexer.hpp"

namespace zebra {

    Lexer::Lexer(const char* file_path) {
        read_source(file_path);
    }
    Lexer::~Lexer() {}

    std::vector<Token> Lexer::scan() {
        try {
            scan_source();
        }catch(SyntaxError& e) {
            e.print();
        }
        return m_tokens;
    }

    void Lexer::scan_source() {
        while (!is_at_end()) {
            char c = next();
            switch(c) {
                //single char tokens
                case '+': add_token(TokenType::PLUS); break;
                case '-': add_token(TokenType::MINUS); break;
                case '/': add_token(TokenType::SLASH); break;
                case '*': add_token(TokenType::STAR); break;
                case ';': add_token(TokenType::SEMICOLON); break;
                case '(': add_token(TokenType::LEFT_PAREN); break;
                case ')': add_token(TokenType::RIGHT_PAREN); break;
                case '{': add_token(TokenType::LEFT_BRACE); break;
                case '}': add_token(TokenType::RIGHT_BRACE); break;
                case '%': add_token(TokenType::MOD); break;
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
                case 'p':
                    if (match("rint")) add_token(TokenType::PRINT);
                    break;
                case 'i':
                    if (match("f")) add_token(TokenType::IF);
                    break;
                case 'e':
                    if (match("lse")) add_token(TokenType::ELSE);
                    break;
                case 't':
                    if (match("rue")) add_token(TokenType::TRUE);
                    break;
                case 'f':
                    if (match("alse")) add_token(TokenType::FALSE);
                    break;
                case 'n':
                    if (match("il")) add_token(TokenType::NIL);
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


    void Lexer::read_string() {
        int start = m_current; //not including quote
        while(!is_at_end() && peek() != '"') {
            next();
        }

        if (peek() != '"') throw SyntaxError(m_line, "Unclosed double quotes.\n");

        next();

        int len = m_current - start - 1; //removing two quotes, but adding on extra space for null terminater
        add_token(TokenType::STRING, m_source.substr(start, len));
    }
    

    void Lexer::read_identifier() {
        int start = m_current - 1;
        while(!is_at_end() && is_alpha(peek())) {
            next();
        }

        int len = m_current - start;
        add_token(TokenType::IDENTIFIER, m_source.substr(start, len));
    }

    bool Lexer::match(const std::string& s) {

        if (s != m_source.substr(m_current, s.length())) {
            return false;
        }

        for (int i = 0; i < (int)(s.length()); i++) next();

        return true;
    }

    void Lexer::read_float_with_leading() {
        int start = m_current - 1;
        while(!is_at_end() && is_numeric(peek())) {
            next();
        }
        int len = m_current - start;
        add_token(TokenType::FLOAT, m_source.substr(start, len));
    }

    void Lexer::read_number() {
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

    bool Lexer::match(char c) {
        if(peek() == c) {
            next();
            return true;
        }
        return false;
    }

    char Lexer::next() {
        m_current++;
        return m_source[m_current - 1];
    }

    char Lexer::peek() {
        assert(m_current < int(m_source.length()));
        return m_source[m_current];
    }

    bool Lexer::is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool Lexer::is_alpha_numeric(char c) {
        return (is_alpha(c) || is_numeric(c));
    }

    bool Lexer::is_at_end() {
        return m_current >= int(m_source.length() - 1);
    }

    bool Lexer::is_numeric(char c) {
        return c >= '0' && c <= '9';
    }

    void Lexer::add_token(TokenType type) {
        m_tokens.emplace_back(Token(type, "", m_line));
    }
    void Lexer::add_token(TokenType type, const std::string& lexeme) {
        m_tokens.emplace_back(Token(type, lexeme, m_line));
    }

    void Lexer::print_source() const {
        std::cout << m_source;
    }

    void Lexer::read_source(const char* file_path) {
        std::string line;
        std::ifstream my_file(file_path);

        if(my_file.is_open()) {
            while (getline(my_file, line)) {
              m_source += line + "\n";
            }
            my_file.close();
        }

    }

}
