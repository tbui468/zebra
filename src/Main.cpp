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
//Call implementation is causing problems with the mixture of Stmt and Expr
//  Make Assign, VarDecl, FunDecl, Call (already expr) into Expr.  Remove combine AssignStmt and AssignExpr into Assign (Expr).
//  then create a ExprStmt (Stmt) that holds an Expr*.  Wrap any expressions with unused values in ExprStmt for better organized Parser code
//  The ExprStmt just calls eval on the expression but throws out the output then reorganize Parser using ExprStmt where possible to simplify code
//In TypeChecker, chang m_types map to m_declarations map (of Stmt*) so that expressions have access to variable types and function return types
//Get function calls working - need a Call object (which we will use again for methods) - Call  is an expression
//
//  try a basic call with now variables if that's easier
//  note: FunDecl creates a function in the environment
//       Call grabs that function, binds the arguments to that function, and then calls the function body
//       Need a none object for no returns; typechecker needs to make sure that Calls with no return can't be assigned to a variable
//make sure to add FUNC_TYPE to variables too
//functions - function objects and function instances, including new scopes, returns using exceptions
//  implement functions in parser - check that it works
//  my_fun :: (a: int, b: float) -> string {} // anonymous function (a: int) -> none {}
//  
//  a: Func = my_fun;
//  a = other_fun;
//  a: int = my_fun(4);
//
//Classes
//  Animal :: Class { //base class declaration (with Class keyword)
//      my_method :: (a: float) -> {
//
//      }
//  }
//  Dog :: Animal { //subclass (up to one inheritance for now)
//  
//  }
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
