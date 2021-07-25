#include <vector>
#include <unordered_map>
#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Stmt.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
#include "TypeChecker.hpp"
//#include "Interpreter.hpp"

//TITLE: Zebra scripting language - 
    //types must match, brackets, semicolons and parentheses are required.  No ambiguity, no shades of gray.  Keeping it black and white.
    //conditions must evaluate to a boolean.  
    //if(4 < 5) OK!  if("dog") X
    //if(true) { print "dog"; } OK if(true) print "dog"; X
    //no nulls or nils or nullptrs, all variables must be defined at declaration time
    //types must match, but casting functions are avaiable for use

//TODO: 
//Get function calls working in interpreter- need a Call object (which we will use again for methods) - Call  is an expression
//
//should functions be first class citizens??? It makes the syntax uglier (and a fun type is required), but it's very useful...
//
//Resolver - functions should capture closure during declaration - this should integrate the TypeChecker (instead of traversing the tree twice)
//  could have the return be a struct of multiple values + a list of Resolver/TypeChecking errors (rather than using exceptions)
//For Lexer, Parser, TypeChecker and Resolver: replace exceptions with error codes - exceptions just cause trouble
    //if more than one return is needed, create a struct that can be return.  Have funtions null out any info not need up higher,
    //and the pass the struct upwards
//Change for loop separators from semicolons to commas(or colons) to make it prettier (and more consistent with function declaraction syntax)
//native cast functions - int(2.3), string(3), float("3.2"), float(3), bool("cat"), 


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: zebra <script>");
    } else {
        zebra::Lexer lexer(argv[1]); 
        std::vector<zebra::Token> tokens = lexer.scan();
       
       
        lexer.print_source();
        
        for (zebra::Token t: tokens) {
            std::cout << t.to_string() << std::endl;
        }


        zebra::Parser parser(tokens);
        std::vector<std::shared_ptr<zebra::Stmt>> ast = parser.parse();

        zebra::AstPrinter printer;        
        for(int i = 0; i < int(ast.size()); i++) {
            printer.print(ast.at(i).get());
        }

        zebra::TypeChecker checker;
        bool passed = checker.check(ast);
/*
        if(passed) {
            zebra::Interpreter interp(ast);
            interp.run();
        } else {
            std::cout << "Type errors found.  Can not run code." << std::endl;
        }*/

    }
    return 0;
}
