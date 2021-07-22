set(Sources
    Main.cpp
    Token.cpp
    TokenArray.cpp
    Parser.cpp
    Lexer.cpp
    )

set(Headers
    TokenType.hpp
    Token.hpp
    TokenArray.hpp
    Parser.hpp
    Lexer.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
