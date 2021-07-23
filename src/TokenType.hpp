#ifndef ZEBRA_TOKEN_TYPE_H
#define ZEBRA_TOKEN_TYPE_H

namespace zebra {
    enum class TokenType {
        //single char tokens
        PLUS, MINUS, STAR, SLASH,
        DOT, SEMICOLON,
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        MOD,
        //double or single char tokens
        EQUAL, EQUAL_EQUAL,
        LESS, LESS_EQUAL,
        GREATER, GREATER_EQUAL,
        BANG,BANG_EQUAL,
        //literals
        INT, FLOAT, STRING, IDENTIFIER,
        //keywords
        PRINT, IF, ELSE,
        TRUE, FALSE,
        AND, OR,
        /*
        RIGHT_ARROW,
        COLON
        FUN,
        RETURN,
        INT_TYPE, ??
        STRING_TYPE, ??
        FLOAT_TYPE, ??
        BOOL_TYPE,  ??
        FOR,
        WHILE,
        NIL,
        ELIF, //new stuff
        BREAK,
        CONTINUE,*/
        SLASH_SLASH,
        EOFILE
    };

}

#endif //ZEBRA_TOKEN_TYPE_H
