#ifndef ZEBRA_TOKEN_TYPE_H
#define ZEBRA_TOKEN_TYPE_H

namespace zebra {
    enum class TokenType {
        //single char tokens
        PLUS, MINUS, STAR, SLASH,
        DOT, SEMICOLON,
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        MOD, COLON, COMMA,
        //double or single char tokens
        EQUAL, EQUAL_EQUAL,
        LESS, LESS_EQUAL,
        GREATER, GREATER_EQUAL,
        BANG,BANG_EQUAL, COLON_COLON,
        RIGHT_ARROW,
        //literals
        INT, FLOAT, STRING, IDENTIFIER, NIL,
        //keywords
        IF, ELSE, WHILE, FOR,
        TRUE, FALSE,
        AND, OR,
        RETURN, CLASS, IMPORT, PRINT,
        //types
        INT_TYPE, FLOAT_TYPE, STRING_TYPE,
        BOOL_TYPE, FUN_TYPE, NIL_TYPE, STRUCT_TYPE,
        /*
        ELIF, //new stuff
        BREAK,
        CONTINUE,*/
        SLASH_SLASH,
        EOFILE
    };

}

#endif //ZEBRA_TOKEN_TYPE_H
