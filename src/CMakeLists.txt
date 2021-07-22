set(Sources
    Main.cpp
    Stmt.hpp
    Expr.hpp
    Token.cpp
    TokenArray.cpp
    StmtList.cpp
    Parser.cpp
    Lexer.cpp
    )

set(Headers
    TokenType.hpp
    Token.hpp
    TokenArray.hpp
    StmtList.hpp
    Parser.hpp
    Lexer.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
