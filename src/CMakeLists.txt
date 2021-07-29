set(Sources
    Main.cpp
    Interpreter.cpp
    Environment.cpp
    Object.cpp
    )

set(Headers
    Stmt.hpp
    Expr.hpp
    TokenType.hpp
    Token.hpp
    Lexer.hpp
    Parser.hpp
    AstPrinter.hpp
    TypeChecker.hpp
    Interpreter.hpp
    Object.hpp
    Environment.hpp
    RuntimeError.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
