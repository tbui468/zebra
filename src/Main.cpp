
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
//      //if / else statements and blocks
        //or
        //and
        //equal !equal
        //four inequalities
        //assignment (int, float, string, bool)
        //scopes


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
