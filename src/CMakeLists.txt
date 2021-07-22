set(Sources
    Main.cpp
    Stmt.hpp
    Expr.hpp
    Token.cpp
    Object.hpp
    Lexer.cpp
    Parser.cpp
    Interpreter.cpp
    AstPrinter.cpp
    )

set(Headers
    TokenType.hpp
    Token.hpp
    Object.cpp
    Lexer.hpp
    Parser.hpp
    Interpreter.hpp
    AstPrinter.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
