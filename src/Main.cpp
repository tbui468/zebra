#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"


//TODO: 
//  Parser - get basic expression or print working first
//      may need a copy constructor for tokens
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


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: zebra <script>");
    } else {
        zebra::Lexer lexer(argv[1]); 
        zebra::TokenArray* t = lexer.scan();
        t->print();
    }
    return 0;
}
