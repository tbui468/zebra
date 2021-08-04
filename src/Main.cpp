#include <vector>
#include <unordered_map>
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AstPrinter.hpp"
#include "Typer.hpp"
#include "Interpreter.hpp"

//TITLE: Zebra scripting language - 
/*
 * Study
 */
//Read PyPY docs to see how they implemented it at a high level - skim what seems interesting for now
//
/*
 * HIGH PRIORITY
 */
//
//InstClass shouldn't really need its own node - it's just a DeclVar with a value being the result of a constructor (function) call
//
//Class dot notation should be able to use GetVar, SetVar and CallFun - rather than having three separate nodes for classes
//  what if user tries to overwrite class methods?  Shouldn't allow this
//
//REPL needs to be possible for a scripting language
//  make all native funtions load - get rid of (or disable) imports for now
//  global variables can be overwritten - I don't like this idea.  Maybe have a keyword (like clear()) to clear environment variables
//  check that it works - print out result of expressions to check basic usability
//  check advanced usability by typing in long scripts (from test files) to see if it works
//
//File manipulation and IO - load all the functions globally for now (can split them to libraries later)
//  read_line / print
//  open_file / close_file
//  read_line / write_line
//
//cast functions - need this for print() function to work
//
//When / Is statements
//
//Enums
//animals: enum {
//  DOG
//  CAT
//  BIRD
//  ZEBRA
//}
//
//b: animal = animals.DOG
//c: int = int(b)
//
//Data structures
//  List, Map - need to integrate types (including inheritance and polymorphism)
//  takes single argument, the data type
//  l: List = List(int)
//  l := List(Animal)
//
//foreach /in
//  foreach i: int in my_list {
//      print(i)
//  }
//
//ranges
//  foreach i: int in 1..10 { //for [1, 10] (inclusive)
//      print(i)
//  }
//
//Multiple return values - will be used for Map
//
//foreach k: string, v: float in my_map {
//  print(k + " " + string(v))
//}
//
//Write a simple tic tac toe or connect 4 program to test usability of zebra (a few 100 lines)
//
//Write a mini language interpreter (a few 1000 lines)
//
//Write tests for error codes - feed in source file and check what kinds of errors come out
//
//When mostly done with Crafting Interpreters, add a compiler between Typer and Interpreter to generate bytecode.  Make Interpreter read in bytecode instead of AST
//  source code -> Lexer -> Parser -> Typer -> Compiler -> Interpreter
//
/*
 * LOW PRIORITY
 */
//
//
//Typer could do two passes through AST - first pass to define classes and functions (anything with :: notation)
//  motivation: don't want to have to forward declare classes and functions if a project is composed of two or more source files
//      import modules
//  and then second pass does the type checking.  This would let up declare classes anywhere.
//  This will be necessary if we want to support multiple files / import files/modules
//  Would need to do a pass on all files in project.  Could call it 'Infer' to infer classes and functions declarations
//  How does Java do thi?
//  source code -> Lexer -> Parser -> Inferer -> Typer -> Compiler -> Interpreter
//
//Don't like the confusing way 'returns' are dealt with in Interpreter / Environment / FunDef / Callables
//
//Block comments (easy)
//
//Infer types when declaring: do this to allow less verbose data structure creation
//  a := 6; //int
//  b := "dog"; //string
//  c := Dog(); //dog instance.  This also allows instatiating for use as an argument
//  d := animal.CAT //enum
//
//All Error types can inherit from same base class Error()
//  printing errors in Main.cpp can just be done by using print() method (rather than calling cout << with all the fields)

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

//            zebra::AstPrinter printer;        
//            printer.print(ast);

            
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
