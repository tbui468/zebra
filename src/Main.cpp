#define _CRT_SECURE_NO_WARNINGS //to silence fopen warning
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

const int MAX_LEXEME_SIZE = 256;


//TODO: 
//  Implement match for keywords
//  remove NUMBER TokenType and replace with INT and FLOAT types
//  implement remaining tokens (up to optional ones)
//  numbers
//      double peek ahead for decimals to allow numbers with leading decimals
//      change to allow trailing decimals too
//  ParseError shouldn't need a be used in Lexer (also get rid of NONE TokenType)
//      Just get the tokens necessary and throw errors in the Parsing stage if the tokens don't match
//  Create a SyntaxError that only shows the line and 
//  Parser - get basic expression or print working first
//  Think about const char* a bit more. Try on godbolt 
//  split into multiple files
//  Get basic arithmetic calculator working using basic C++ (essentially C with classes)
//  Use any C++ libraries if necessary, but replace them when possible
//   Lexer
//     NUMBER, STAR, SLASH, PLUS, MINUS
//   Parser
//     Expr
//       Literal, Binary
//   Interpreter
//     get it evaluating expressions (and use printf to get it showing up on console)

namespace zebra {


    enum class TokenType {
        NUMBER,
        FLOAT,
        INT,
        STRING,
        STAR,
        SLASH,
        PLUS,
        MINUS,
        DOT,
        SEMICOLON,
        LEFT_PAREN,
        RIGHT_PAREN,
        IDENTIFIER,
        EQUAL,
        EQUAL_EQUAL,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        BANG_EQUAL,
        BANG,
        RIGHT_ARROW,
        COLON,
        ELSE, //new from here (except print and if)  This should be part of hash table
        PRINT,
        IF,
        FUN,
        RETURN,
        INT_TYPE,
        STRING_TYPE,
        FLOAT_TYPE,
        BOOL_TYPE,
        FOR,
        WHILE,
        TRUE,
        FALSE,
        AND,
        OR,
        NIL,
        ELIF, //new stuff
        BREAK,
        CONTINUE,
        EOFILE
    };


    struct Token {
        public:
            TokenType m_type;
            int m_length;
            char m_lexeme[MAX_LEXEME_SIZE] = {0};
            int m_line;
        public:
            Token(): Token(TokenType::STRING, nullptr, 0, 0) {}
            Token(TokenType type, const char* start, int len, int line): m_type(type), m_line(line) {
                if(start) strncpy(m_lexeme, start, len);
            }
            ~Token() {}
    };


    class ParseError {
        private:
            Token m_token;
            const char* m_message;
        public:
            ParseError(Token token, const char* message): m_token(token), m_message(message){}
            ~ParseError() {}
            void print() {
                printf("[Line %d] Error: %s", m_token.m_line, m_message);
            }
    };
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

    //starts at a fixed size and doubles in size if necessary
    //and it copies over data if needed
    class TokenArray {
        private:
            Token* m_tokens;
            const int DEFAULT_SIZE = 8;
            int m_capacity = DEFAULT_SIZE;
            int m_size = 0;
        public:
            TokenArray() {
                init();
            }
            ~TokenArray() {
                delete[] m_tokens;
            }
            Token at(int index) const {
                assert(index < m_size);
                return m_tokens[index];
            }
            int size() const {
                return m_size;
            }
            void add(const Token& token) {
                if (m_size == m_capacity) {
                    double_size();
                }
                m_tokens[m_size] = token;
                m_size++;
            }

            void print() {
                for (int i = 0; i < m_size; i++) {
                    switch(m_tokens[i].m_type) {
                        case TokenType::NUMBER:         printf("NUMBER [%s]\n", m_tokens[i].m_lexeme); break;
                        case TokenType::STRING:         printf("STRING [\"%s\"]\n", m_tokens[i].m_lexeme); break;
                        case TokenType::STAR:           printf("STAR\n");                               break;
                        case TokenType::SLASH:          printf("SLASH\n");                              break;
                        case TokenType::PLUS:           printf("PLUS\n");                               break;
                        case TokenType::MINUS:          printf("MINUS\n");                              break;
                        case TokenType::DOT:            printf("DOT\n");                                break;
                        case TokenType::PRINT:          printf("PRINT\n");                              break;
                        case TokenType::SEMICOLON:      printf("SEMICOLON\n");                          break;
                        case TokenType::LEFT_PAREN:     printf("LEFT_PAREN\n");                         break;
                        case TokenType::RIGHT_PAREN:    printf("RIGHT_PAREN\n");                        break;
                        case TokenType::IDENTIFIER:     printf("IDENTIFIER [%s]\n", m_tokens[i].m_lexeme);                         break;
                        case TokenType::IF:             printf("IF\n");                        break;
                        case TokenType::EQUAL:printf("EQUAL\n");                                break;
                        case TokenType::EQUAL_EQUAL:printf("EQUAL_EQUAL\n");                                break;
                        case TokenType::LESS:printf("LESS\n");                                break;
                        case TokenType::GREATER:printf("GREATER\n");                                break;
                        case TokenType::LESS_EQUAL:printf("LESS_EQUAL\n");                                break;
                        case TokenType::GREATER_EQUAL:printf("GREATER_EQUAL\n");                                break;
                        case TokenType::BANG_EQUAL:printf("BANG_EQUAL\n");                                break;
                        case TokenType::BANG:printf("BANG\n");                                break;
                        case TokenType::RIGHT_ARROW:printf("RIGHT_ARROW\n");                                break;
                        case TokenType::COLON:printf("COLON\n");                                break;
                        case TokenType::EOFILE:printf("EOFILE\n");                                break;
                    }
                } 
            }
        private:
            void init() {
                m_tokens = new Token[m_capacity];
            }

            void double_size() {
                m_capacity *= 2;
                Token* new_array = new Token[m_capacity];
                memcpy((void*)new_array, (const void*)m_tokens, sizeof(Token) * m_size);
                delete[] m_tokens;
                m_tokens = new_array;
            }
    };

    class Lexer {
        private:
            char* m_source;
            TokenArray m_tokens;
            int m_length;
            int m_current = 0;
            int m_line = 1;
        public:
            Lexer(const char* file_path) {
                read_source(file_path);
            }
            ~Lexer() {
                delete[] m_source;
            }

            void scan() {
                try {
                    scan_source();
                }catch(SyntaxError& e) {
                    e.print();
                }
            }

            void scan_source() {
                while (!is_at_end()) {
                    char c = next();

                    switch(c) {
                        case '+': add_token(TokenType::PLUS); break;
                        case '-': 
                            if (match('>')) add_token(TokenType::RIGHT_ARROW);
                            else add_token(TokenType::MINUS);
                            break;
                        case '/': add_token(TokenType::SLASH); break;
                        case '*': add_token(TokenType::STAR); break;
                        case ';': add_token(TokenType::SEMICOLON); break;
                        case '(': add_token(TokenType::LEFT_PAREN); break;
                        case ')': add_token(TokenType::RIGHT_PAREN); break;
                        case '.': add_token(TokenType::DOT); break;
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
                        case '"': read_string(); break;
                        case '\n': m_line++;
                        case '\r':
                        case ' ':
                            break;
                        default:
                            if (is_numeric(c)) {
                                read_number();
                            } else if (is_alpha(c)) {
                                //read_keywords();
                            } else {
                                throw SyntaxError(m_line, "Invalid character.\n");
                            }
                            break;
                    }
                }
                add_token(TokenType::EOFILE);
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
                assert(m_current < m_length);
                return m_source[m_current];
            }

            bool is_alpha(char c) {
                return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
            }

            bool is_alpha_numeric(char c) {
                return (is_alpha(c) || is_numeric(c));
            }

            void read_string() {
                int start = m_current; //not including quote
                while(!is_at_end() && peek() != '"') {
                    next();
                }

                if (peek() != '"') throw SyntaxError(m_line, "Unclosed double quotes.\n");

                next();

                int len = m_current - start - 1; //removing two quotes, but adding on extra space for null terminater
                if (len >= MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
                add_token(TokenType::STRING, &m_source[start], len);
            }

            void read_keywords() {
                int start = m_current - 1;
                while(!is_at_end() && is_alpha(peek())) {
                    next();
                }

                int len = m_current - start;
                if (len >= MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");

                if (strcmp(word, "print") == 0) {
                    add_token(TokenType::PRINT);
                } else if (strcmp(word, "if") == 0) {
                    add_token(TokenType::IF);
                } else {
                    add_token(TokenType::IDENTIFIER, &m_source[start], len);
                }
            }

            bool match_chars(const char* other) {

            }


            void read_number() {
                int start = m_current - 1;
                while(!is_at_end() && is_numeric(peek())) {
                    next();
                }

                //allows trailing decimals - find a way to get rid of this
                if (!is_at_end() && peek() == '.') {
                    next();               
                    if(!is_at_end() && !is_numeric(peek())) {
                        throw SyntaxError(m_line, "Numbers can't end with a trailing decimal.\n");
                    }
                    while(!is_at_end() && is_numeric(peek())) {
                        next();
                    }
                }

                int len = m_current - start;
                if (len >= MAX_LEXEME_SIZE) throw SyntaxError(m_line, "Max identifier or number length is 256 characters.\n");
                add_token(TokenType::NUMBER, &m_source[start], len);
            }

            bool is_at_end() {
                return m_current >= m_length - 1;
            }

            bool is_numeric(char c) {
                return c >= '0' && c <= '9';
            }

            void add_token(TokenType type) {
                m_tokens.add(Token(type, nullptr, 0, m_line));
            }
            void add_token(TokenType type, const char* start, int len) {
                m_tokens.add(Token(type, start, len, m_line));
            }

            TokenArray* get_tokens() {
                return &m_tokens; 
            }

            //just for debugging
            void print_source() const {
                printf("%s", m_source);
            }
        private:
            void read_source(const char* file_path) {
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

    };
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: zebra <script>");
    } else {
        zebra::Lexer lexer(argv[1]); 
        lexer.scan();
        zebra::TokenArray* t = lexer.get_tokens();
        t->print();
    }
    return 0;
}
