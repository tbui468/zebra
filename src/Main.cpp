#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"


//TODO: 
//  Parser - get basic expression or print working first
//      Need a StmtList with variable length to hold statements
//      fill out recursive descent for expressions (binary, unary at least)
//      move Printing AST into own class using visitor pattern
//      move Parser body into cpp file
//  Interpreter
//      Take in AST as input and execute statements and evaluate expressions
//  Get basic arithmetic calculator working using basic C++ (essentially C with classes)
//  Use any C++ libraries if necessary, but replace them when possible
//   Lexer
//     NUMBER, STAR, SLASH, PLUS, MINUS
//   Parser
//     Expr
//       Literal, Binary
//   Interpreter
//     get it evaluating expressions (and use printf to get it showing up on console)
//   TokenArray is allocating memory for Token pointers - should really allocate memory
//      for the Token themeselves for quicker/efficient access since all tokens are same size (unlike Stmt)


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: zebra <script>");
    } else {
        zebra::Lexer lexer(argv[1]); 
        zebra::TokenArray* t = lexer.scan();
        t->print();
        
        zebra::Parser parser(t);
        zebra::StmtList* sl = parser.parse();
        if(sl) {
            for(int i = 0; i < sl->size(); i++) {
                zebra::Parser::print(sl->at(i));
            }
        }

    }
    return 0;
}
