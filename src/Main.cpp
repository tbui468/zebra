
#include <vector>
#include <unordered_map>
#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Stmt.hpp"
#include "AstPrinter.hpp"
#include "Interpreter.hpp"
#include "TypeChecker.hpp"
#include "DataType.hpp"

//TITLE: Zebra scripting language - 
    //types must match, brackets, semicolons and parentheses are required.  No ambiguity, no shades of gray.  Keeping it black and white.
    //conditions must evaluate to a boolean.  
    //if(4 < 5) OK!  if("dog") X
    //if(true) { print "dog"; } OK if(true) print "dog"; X
    //no nulls or nils or nullptrs, using undefined variables will throw an error

//TODO: 
//must declare and define all variables at the same time
//create two tables, one with <variable name, type> and <variable name, value> for the Environment class (for scopes)
//for TypeChecker, create a <variable name, type> table to check if the type is valid when assigning or other stuff
//  
//scopes for variables
//while loops
//for loops
//Resolver - variables and functions too
//How should errors propagate?  Parsing, Type checking and Runtime errors
//  should the entire program abort if there's a problem?  YES


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
            printer.print(ast.at(i));
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
