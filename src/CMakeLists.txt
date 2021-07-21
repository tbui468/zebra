set(Sources
    Main.cpp
    Parser.cpp
    )

set(Headers
    Parser.hpp
    )

add_executable(
    Zebra
    ${Headers}
    ${Sources}
    )
