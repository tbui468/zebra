#define _CRT_SECURE_NO_WARNINGS //to silence fopen warning
#include <stdio.h>
#include <string.h>
#include <assert.h>


//TODO: 
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
        STAR,
        SLASH,
        PLUS,
        MINUS,
        DOT
    };

    struct Token {
        public:
            TokenType m_type;
            double m_literal;
            int m_line;
        public:
            Token(): m_type(TokenType::NONE), m_literal(0.0) {}
            Token(TokenType type): m_type(type), m_literal(0.0) {}
            Token(TokenType type, double literal): m_type(type), m_literal(literal) {}
            ~Token() {}
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
                printf("size: %d", m_size);
                printf("capacity: %d\n", m_capacity);
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
                while (m_current < m_length) {
                    char c = m_source[m_current];

                    switch(c) {
                        case '+': m_tokens.add(Token(TokenType::PLUS)); m_current++; break;
                        case '-': m_tokens.add(Token(TokenType::MINUS)); m_current++; break;
                        case '/': m_tokens.add(Token(TokenType::SLASH)); m_current++ ;break;
                        case '*': m_tokens.add(Token(TokenType::STAR)); m_current++; break;
                        case '\n': m_line++; m_current++; break;
                        case '\r':
                        case '\0':
                        case ' ':
                            break;
                        default: //number or dot  advance until not a number or dot
                            double literal = read_number();
                            m_tokens.add(Token(TokenType::NUMBER, literal));
                            break;
                    }
                }
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
    }
    return 0;
}
