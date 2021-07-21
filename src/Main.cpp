#define _CRT_SECURE_NO_WARNINGS //to silence fopen warning
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


//TODO: 
//  implement hash map - look at Crafting Interpreters guide
//  how to keep char* arrays alive?  Can I call new?  
//  get read number working.  No leading or trailing decimals for now.
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
        NONE,
        NUMBER,
        STRING,
        STAR,
        SLASH,
        PLUS,
        MINUS,
        DOT,
        PRINT,
        SEMICOLON,
        LEFT_PAREN,
        RIGHT_PAREN,
        IDENTIFIER,
        IF
    };

    struct Token {
        public:
            TokenType m_type;
            const char* m_literal;
            const char* m_lexeme;
            int m_line;
        public:
            Token(): m_type(TokenType::NONE), m_literal(""), m_lexeme(""), m_line(0) {}
            Token(TokenType type, const char* literal, const char* lexeme, int line): m_type(type), m_literal(literal), m_lexeme(lexeme), m_line(line) {}
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
                        case TokenType::NONE:           printf("NONE\n");                               break;
                        case TokenType::NUMBER:         printf("NUMBER [%f]\n", atof(m_tokens[i].m_literal)); break;
                        case TokenType::STRING:         printf("STRING [\"%s\"]\n", m_tokens[i].m_literal); break;
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
                }catch(ParseError& e) {
                    e.print();
                }
            }

            void scan_source() {
                while (!is_at_end()) {
                    char c = next();

                    switch(c) {
                        case '+': add_token(TokenType::PLUS); break;
                        case '-': add_token(TokenType::MINUS); break;
                        case '/': add_token(TokenType::SLASH); break;
                        case '*': add_token(TokenType::STAR); break;
                        case ';': add_token(TokenType::SEMICOLON); break;
                        case '(': add_token(TokenType::LEFT_PAREN); break;
                        case ')': add_token(TokenType::RIGHT_PAREN); break;
                        case '.': add_token(TokenType::DOT); break;
                        case '"': read_string(); break;
                        case '\n': m_line++;
                        case '\r':
                        case '\0':
                        case ' ':
                            break;
                        default:
                            if (is_numeric(c)) {
                                read_number();
                            } else if (is_alpha(c)) {
                                read_keywords();
                            } else {
                                throw ParseError(Token(TokenType::NONE, "", "", m_line), "Invalid token.");
                            }
                            break;
                    }
                }
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

                if (peek() != '"') throw ParseError(Token(TokenType::NONE, "", "", m_line), "Unclosed double quotes.");

                next();

                int size = m_current - start - 1;
                char* word = new char[size];
                for (int i = 0; i < size; i++) {
                    word[i] = m_source[start + i];
                }
                word[size] = '\0';
                add_token(TokenType::STRING, word, "");
            }

            void read_keywords() {
                int start = m_current - 1;
                while(!is_at_end() && is_alpha(peek())) {
                    next();
                }
                int size = m_current - start;
                char* word = new char[size + 1];
                for (int i = 0; i < size; i++) {
                    word[i] = m_source[start + i];
                }
                word[size] = '\0';

                if (strcmp(word, "print") == 0) {
                    add_token(TokenType::PRINT, "", "");
                } else if (strcmp(word, "if") == 0) {
                    add_token(TokenType::IF, "", "");
                } else {
                    add_token(TokenType::IDENTIFIER, "", word);
                }
            }


            void read_number() {
                int start = m_current - 1;
                while(!is_at_end() && is_numeric(peek())) {
                    next();
                }

                //if decimal is next, continuing reading 
                if (!is_at_end() && peek() == '.') {
                    next();               
                    if(!is_at_end() && !is_numeric(peek())) {
                        throw ParseError(Token(TokenType::NONE, "", "", m_line), "Numbers can't end with a trailing decimal.");
                    }
                    while(!is_at_end() && is_numeric(peek())) {
                        next();
                    }
                }

                int size = m_current - start;
                char* num = new char[size + 1];
                for (int i = 0; i < size; i++) {
                    num[i] = m_source[start + i];
                }
                num[size] = '\0';
                add_token(TokenType::NUMBER, num, "");
            }

            bool is_at_end() {
                return m_current >= m_length - 1;
            }

            bool is_numeric(char c) {
                return c >= '0' && c <= '9';
            }

            void add_token(TokenType type) {
                m_tokens.add(Token(type, "", "", m_line));
            }
            void add_token(TokenType type, const char* literal, const char* lexeme) {
                m_tokens.add(Token(type, literal, lexeme, m_line));
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
