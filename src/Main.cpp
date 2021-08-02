#include <vector>
#include <unordered_map>
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
//#include "Typer.hpp"
#include "Interpreter.hpp"

//TITLE: Zebra scripting language - 
    //types must match, brackets, semicolons and parentheses are required.  No ambiguity, no shades of gray.  Keeping it black and white.
    //conditions must evaluate to a boolean.  
    //if(4 < 5) OK!  if("dog") X
    //if(true) { print "dog"; } OK if(true) print "dog"; X
    //no nulls or nils or nullptrs, all variables must be defined at declaration time
    //types must match, but casting functions are avaiable for use

//TODO: 
//class inheritance
//  fields are inherited, but not functions
//
//
//Animal :: class {
//  species: string = "mammal"
//}
//
//Dog :: class < Animal {
//    name: string = "Mittens"
//    age: int = 3
//
//    speak :: () -> {
//        print("My name is " + name)
//        ->
//    }
//
//    dog_age :: () -> int {
//        -> age * 7
//    }
//}
//
//Typer - this will be a big project.  May require restructing of Parser, Interpreter and Environments
//
//Array, List, Map
//
//Polymorphism
//
//Redo TypeChecker
//  replace exceptions with return codes (or class data field that holds list of accrued errors)
//  Typechecker needs to deal with three types of data: primitive types, functions and structs
//
//All Error types can inherit from same base class Error()
//  printing errors in Main.cpp can just be done by using print() method (rather than calling cout << with all the fields)
//
//
//Infer types when declaring:
//a := 6; //int
//b := "dog"; //string
//c := Dog(); //dog instance.  This also allows instatiating for use as an argument
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

            zebra::Lexer::print_tokens(tokens);

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

/*            
            zebra::Typer typer;
            zebra::ResultCode type_result = typer.type(ast);

            if (type_result != zebra::ResultCode::SUCCESS) {
                std::vector<zebra::TypeError> errors = typer.get_errors();
                for (zebra::TypeError error: errors) {
                    std::cout << "[" << error.m_token.to_string() << "]" << error.m_message << std::endl;
                }
                return 1;
            }*/

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
