#include <vector>
#include <unordered_map>
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
#include "Typer.hpp"
#include "Interpreter.hpp"

//TITLE: Zebra scripting language - 
    //types must match, brackets, semicolons and parentheses are required.  No ambiguity, no shades of gray.  Keeping it black and white.
    //conditions must evaluate to a boolean.  
    //if(4 < 5) OK!  if("dog") X
    //if(true) { print "dog"; } OK if(true) print "dog"; X
    //no nulls or nils or nullptrs, all variables must be defined at declaration time
    //types must match, but casting functions are avaiable for use

//TODO: 
//Typer - this will be a big project.  Will require restructing of Parser, Interpreter and Environments
//  Finish Type checking functions and function calls and returns 
//  Control Flow
//      do this before Variables and Functions since it'll be easier
//  Variables and Functions
//  Classes
//
//  What types are there?  primitive, classes and functions
//  What do we check? - will need to go through each expression and maybe also create new environments??? (I want to avoid this if possible)
//
//Infer types when declaring: do this to allow less verbose data structure creation
//  a := 6; //int
//  b := "dog"; //string
//  c := Dog(); //dog instance.  This also allows instatiating for use as an argument
//
//Enums
//
//Data structures
//  Array, List, Map - need to integrate types (including inheritance and polymorphism)
//  takes single argument, the data type
//  l: List = List(int)
//  l := List(Animal)
//  a: Array = Array(int)
//
//
//Write a simple tic tac toe or connect 4 program to test usability of zebra
//
//Write tests for error codes - feed in source file and check what kinds of errors come out
//
//All Error types can inherit from same base class Error()
//  printing errors in Main.cpp can just be done by using print() method (rather than calling cout << with all the fields)
//
//native cast functions - int(2.3), string(3), float("3.2"), float(3), bool("cat"), print("dog"), print(23.2) should be a native function too

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: zebra <script>");
    } else {

        for (int i = 1; i < argc; i++) {

            zebra::Lexer lexer(argv[i]); 
            std::vector<zebra::Token> tokens;
            zebra::ResultCode scan_result = lexer.scan(tokens); //this should return a result code

            if (scan_result != zebra::ResultCode::SUCCESS) {
                for (zebra::SyntaxError error: lexer.get_errors()) {
                    error.print();
                }
                return 1;
            }

//            zebra::Lexer::print_tokens(tokens);

            zebra::Parser parser(tokens);
            std::vector<std::shared_ptr<zebra::Expr>> ast;
            zebra::ResultCode parse_result = parser.parse(ast);

            if (parse_result != zebra::ResultCode::SUCCESS) {
                for (zebra::ParseError error: parser.get_errors()) {
                    error.print();
                }
                return 1;
            }

            zebra::AstPrinter printer;        
            printer.print(ast);

            
            zebra::Typer typer;
            zebra::ResultCode type_result = typer.type(ast);

            if (type_result != zebra::ResultCode::SUCCESS) {
                std::vector<zebra::TypeError> errors = typer.get_errors();
                for (zebra::TypeError error: errors) {
                    error.print();
                }
                return 1;
            }

            zebra::Interpreter interp;
            zebra::ResultCode run_result = interp.run(ast);

            if (run_result != zebra::ResultCode::SUCCESS) {
                for (zebra::RuntimeError error: interp.get_errors()) {
                    error.print();
                }
                return 1;
            }
        }

    }
    return 0;
}
