
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
//Assume all variables are defined when used for nor
//create two tables, one with <variable name, type> and <variable name, value>, both in env.
//What was the point of the resolver? One was to know which scope to find the variable value
//also need to check if variable is defined before using it
//
//a: int = 1;
//{
//  print a; //1
//  a: int = 2;
//  print a; //2
//}
//
//assignment with types in front (int, float, string, bool)
//  Declaration: public Stmt { //put into hash table in environment
//      Token m_name; //the type and lexeme is saved in here - just need to check 
//      std::shared_ptr<Expr> m_value; //need to know if variable is defined before we use it (need the resolver here?)
//  }
//
//  only update the hash table in environment if data type in m_name is same as data type in m_value 
//  Assign: public Stmt { //update hash table in environment - make sure that m_name.m_type == the type of the expression
//      Token m_name;
//      std::shared_ptr<Expr> m_value; //may be nullptr if type is followed by 
//  }
//
//  Variable: public Expr { //look up in values hash table in environment - how to know
//      Token m_name;
//  }
//
//  a: int = 1 + 2;
//  b: int = a + 4;
//  c: int;
//  c = 10 - b;
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
/*
        zebra::TypeChecker checker;
        bool passed = checker.check(ast);

        if(passed) {
            zebra::Interpreter interp(ast);
            interp.run();
        } else {
            std::cout << "Type errors found.  Can not run code." << std::endl;
        }*/

    }
    return 0;
}
