
#include <vector>
#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Stmt.hpp"
#include "AstPrinter.hpp"
#include "Interpreter.hpp"


//TODO: 
//  Interpreter
//      get if / else statements (and blocks and scopes) working
        //or
        //and
        //equal !equal
        //four inequalities
//      use raw pointers for now - issue with interpeter
//          we need Objects to live a long time (for putting functions and variables into lookup table)
//          but then who frees the memory for Objects that don't need to go into the table???
//      Implement visitor pattern and create AstPrinter class that extends the visitor interface for both Expr and Stmt
//          once that works, use the interfaces on Interpreter
//      Take in AST as input and execute statements and evaluate expressions
//      Dynamically type stuff for now - make object class
//      Put off type-checking for now
//  Parser - get basic expression or print working first
//      Need a StmtList with variable length to hold statements
//      fill out recursive descent for expressions (binary, unary at least)
//      move Printing AST into own class using visitor pattern
//      move Parser body into cpp file
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
        std::vector<zebra::Token> tokens = lexer.scan();
        /*
        lexer.print_source();
        for (zebra::Token t: tokens) {
            std::cout << t.to_string() << std::endl;
        }*/
       
        zebra::Parser parser(tokens);
        std::vector<zebra::Stmt*> sl = parser.parse();
        /*
        zebra::AstPrinter printer;        
        for(int i = 0; i < int(sl.size()); i++) {
            printer.print(*(sl.at(i)));
        }*/

        zebra::Interpreter interp(sl);
        interp.run();

    }
    return 0;
}
