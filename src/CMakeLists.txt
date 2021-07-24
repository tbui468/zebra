set(Sources
    Main.cpp
    )

set(Headers
    Stmt.hpp
    Expr.hpp
    TokenType.hpp
    Token.hpp
    Object.hpp
    Lexer.hpp
    Parser.hpp
    Interpreter.hpp
    AstPrinter.hpp
    TypeChecker.hpp
    Environment.hpp
    RuntimeError.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
