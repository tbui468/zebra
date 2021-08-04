set(Sources
    Main.cpp
    Interpreter.cpp
    Environment.cpp
    Object.cpp
    )

set(Headers
    Expr.hpp
    TokenType.hpp
    DataType.hpp
    Token.hpp
    Lexer.hpp
    Parser.hpp
    AstPrinter.hpp
    #Typer.hpp
    Interpreter.hpp
    Object.hpp
    Environment.hpp
    ResultCode.hpp
    Library.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
