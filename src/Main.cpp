#include <vector>
#include <unordered_map>
#include "TokenType.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Stmt.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
#include "TypeChecker.hpp"
#include "Interpreter.hpp"

//TITLE: Zebra scripting language - 
    //types must match, brackets, semicolons and parentheses are required.  No ambiguity, no shades of gray.  Keeping it black and white.
    //conditions must evaluate to a boolean.  
    //if(4 < 5) OK!  if("dog") X
    //if(true) { print "dog"; } OK if(true) print "dog"; X
    //no nulls or nils or nullptrs, all variables must be defined at declaration time
    //types must match, but casting functions are avaiable for use

//TODO: 
//Make Call into a statement that optionally can be wrapped in an expression
//Make FunDecls into statements since they shouldn't return anything (anonymous functions can do that when they're implemented"
//  current issue: Making FunDecls into statements breaks TypeChecker, since variables are stored as Expr* for type checking
//      VarDecl, FunDecl and StructDecl all need to be stored eventually - VarDecl is an expression since we allow decl/assignments to return the value
//          FunDecl and Struct Decl are pure statements with no return value (anonymous functions will be an expression)
//      Need to store node pointers in TypeChecker.m_variables (and not just types) because FunDecl node parameters need to be type checked againt Call node arguments
//  Structs will also be statements
//  
//Structs
//      Dog :: struct {
//          name: string = "default";
//          age: int = 2;
//          height: float = 23.2;
//      }
//
//      d1: Dog("mittens", 3, 2.3);
//      d1.age = 20;
//      print d1.name;
//
//      d2: Dog("bubbles", 1, 32.);
//      d2 = d1;
//FunDecls (unlike VarDecls) shouldn't return anything - change them from expressions to statements.  The same as structs;
//Look up edges cases in Jlox book, and write tests to see if my code passes them - if not, then fix the bugs
//For Lexer, Parser, TypeChecker and Resolver: replace exceptions with error codes - exceptions just cause trouble
    //if more than one return is needed, create a struct that can be return.  Have funtions null out any info not need up higher,
    //and the pass the struct upwards
//TypeChecker should return error is a function with a none-return type is used as an expression
//Change for loop separators from semicolons to commas(or colons) to make it prettier (and more consistent with function declaraction syntax)
//native cast functions - int(2.3), string(3), float("3.2"), float(3), bool("cat"), print("dog"), print(23.2) should be a native function too

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
        std::vector<std::shared_ptr<zebra::Stmt>> ast = parser.parse();
/*
        zebra::AstPrinter printer;        
        for(int i = 0; i < int(ast.size()); i++) {
            printer.print(ast.at(i).get());
        }*/

        zebra::TypeChecker checker;
        bool passed = checker.check(ast);

        if(passed) {
            zebra::Interpreter interp(ast);
            interp.run();
        } else {
            std::cout << "Type errors found.  Can not run code." << std::endl;
        }

    }
    return 0;
}
