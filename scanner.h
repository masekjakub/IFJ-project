/**
 * @file scanner.h
 * @authors Jakub Mašek, Filip Polomski, Martin Zelenák, Vojtěch Kuchař
 * @brief header file for scanner.h
 * @version 1.0
 * @date 2022-10-27
 */

#ifndef _IFJ_SCANNER
#define _IFJ_SCANNER

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "error.h"
#include "dynamicString.h"

typedef enum {
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_STRING,
    KEYWORD_FUNCTION,
    KEYWORD_VOID,
    KEYWORD_RETURN

}KeyWord;

typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ID,
    TYPE_FUNID,
    TYPE_LABEL,
    TYPE_KEYWORD,
    TYPE_EOF,
    TYPE_DECLARE_ST, // declare strict types
    TYPE_QMARK,      // for ?int var
    TYPE_COMM,       // // or /*

    // Operators
    TYPE_ADD,        // +
    TYPE_SUB,        // -
    TYPE_MUL,        // *
    TYPE_DIV,        // /
    TYPE_MOD,        // %
    TYPE_ASSIGN,     // =
    TYPE_EQTYPES,    // ===
    TYPE_NOTEQTYPES, // !==
    TYPE_LESS,       // <
    TYPE_GREATER,    // >
    TYPE_LESSEQ,     // <=
    TYPE_GREATEREQ,  // >=
    TYPE_CONCAT,     // .

    // Special symbols
    TYPE_LBRACKET,  // (
    TYPE_RBRACKET,  // )
    TYPE_LBRACES,   // {
    TYPE_RBRACES,   // }
    TYPE_DOLLAR,    // $
    TYPE_AMPERSAND, // &
    TYPE_NEG,       // !
    TYPE_SEMICOLON, // ;
    TYPE_COMMA,     // ,
    TYPE_COLON,     // :
    TYPE_BEGIN,     // <?php
    TYPE_END,       // ?>

    TYPE_NULL,
    TYPE_UNDEF,
    TYPE_LESSPREC,
    TYPE_STACKEMPTY,
    TYPE_LEXERR,
    TYPE_EXPR

} TokenType;

typedef union
{
    KeyWord keyword;
    int intV;
    double doubleV;
    DynamicString *dString;
    
}TokenAttribute;

typedef struct
{
    TokenType type;
    TokenAttribute attribute;
    int rowNumber;
}Token;

typedef enum
{
    STATE_START,
    STATE_STRING,
    STATE_VAR,
    STATE_ID,
    STATE_INTEGER,
    STATE_FLOAT
    //STATE_EPILOG
} State;

void setSourceFile(FILE *);
bool isKeyword(DynamicString *, Token*);
DynamicString *octNumber(DynamicString*);
DynamicString *hexNumber(DynamicString*);
Token getToken();


#endif