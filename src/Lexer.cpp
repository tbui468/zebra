#define _CRT_SECURE_NO_WARNINGS //to silence fopen warning
#include <assert.h>
#include <string.h>
#include "Lexer.hpp"

namespace zebra {

    Lexer::Lexer(const char* file_path) {
        read_source(file_path);
    }
    Lexer::~Lexer() {
        delete[] m_source;
    }

    TokenArray* Lexer::scan() {
        try {
            scan_source();
        }catch(SyntaxError& e) {
            e.print();
        }
        return &m_tokens;
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
        if (len >= Lexer::MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
        add_token(TokenType::STRING, &m_source[start], len);
    }
    

    void Lexer::read_identifier() {
        int start = m_current - 1;
        while(!is_at_end() && is_alpha(peek())) {
            next();
        }

        int len = m_current - start;
        if (len >= Lexer::MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");

        add_token(TokenType::IDENTIFIER, &m_source[start], len);
    }

    bool Lexer::match(const char* s) {
        int len = strlen(s);

        //check m_current and compare to s
        for (int i = 0; i < len; i++) {
            if ((m_current + i) >= (m_length - 1)) return false;
            if (s[i] != m_source[m_current + i]) return false;
        }

        for (int i = 0; i < len; i++) next();

        return true;
    }

    void Lexer::read_float_with_leading() {
        int start = m_current - 1;
        while(!is_at_end() && is_numeric(peek())) {
            next();
        }
        int len = m_current - start;
        if (len >= Lexer::MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
        add_token(TokenType::FLOAT, &m_source[start], len);
    }

    void Lexer::read_number() {
        int start = m_current - 1;
        while(!is_at_end() && is_numeric(peek())) {
            next();
        }

        //integer
        if (!is_at_end() && peek() != '.') {
            int len = m_current - start;
            if (len >= Lexer::MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
            add_token(TokenType::INT, &m_source[start], len);
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
        if (len >= Lexer::MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
        add_token(TokenType::FLOAT, &m_source[start], len);
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
        assert(m_current < m_length);
        return m_source[m_current];
    }

    bool Lexer::is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool Lexer::is_alpha_numeric(char c) {
        return (is_alpha(c) || is_numeric(c));
    }

    bool Lexer::is_at_end() {
        return m_current >= m_length - 1;
    }

    bool Lexer::is_numeric(char c) {
        return c >= '0' && c <= '9';
    }

    void Lexer::add_token(TokenType type) {
        m_tokens.add(Token(type, nullptr, 0, m_line));
    }
    void Lexer::add_token(TokenType type, const char* start, int len) {
        m_tokens.add(Token(type, start, len, m_line));
    }

    void Lexer::print_source() const {
        printf("%s", m_source);
    }

    void Lexer::read_source(const char* file_path) {
        FILE* f = fopen(file_path, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            m_length = ftell(f) + 1;
            fseek(f, 0, SEEK_SET); // == rewind(f)
            m_source = new char[m_length];
            fread(m_source, 1, m_length - 1, f);
            m_source[m_length - 1] = '\0';
            fclose(f);
        }
    }

}
