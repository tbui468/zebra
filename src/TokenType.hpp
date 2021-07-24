#ifndef ZEBRA_TOKEN_TYPE_H
#define ZEBRA_TOKEN_TYPE_H

namespace zebra {
    enum class TokenType {
        //single char tokens
        PLUS, MINUS, STAR, SLASH,
        DOT, SEMICOLON,
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        MOD, COLON,
        //double or single char tokens
        EQUAL, EQUAL_EQUAL,
        LESS, LESS_EQUAL,
        GREATER, GREATER_EQUAL,
        BANG,BANG_EQUAL,
        //literals
        INT, FLOAT, STRING, IDENTIFIER,
        //keywords
        PRINT, IF, ELSE, WHILE, FOR,
        TRUE, FALSE,
        AND, OR,
        INT_TYPE, FLOAT_TYPE, STRING_TYPE, BOOL_TYPE,
        /*
        RIGHT_ARROW,
        FUN,
        RETURN,
        INT_TYPE, ??
        STRING_TYPE, ??
        FLOAT_TYPE, ??
        BOOL_TYPE,  ??
        NIL,
        ELIF, //new stuff
        BREAK,
        CONTINUE,*/
        SLASH_SLASH,
        EOFILE
    };

}

#endif //ZEBRA_TOKEN_TYPE_H
