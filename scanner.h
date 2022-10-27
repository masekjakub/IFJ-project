/**
 * @file scanner.h
 * @authors Jakub Mašek, Filip Polomski, Martin Zelenák, Vojtěch Kuchař
 * @brief header file for scanner.h
 * @version 0.1
 * @date 27-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _IFJ_SCANNER
#define _IFJ_SCANNER

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "dynamicString.h"

typedef enum {
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_STRING,
    KEYWORD_NULL,
    KEYWORD_FUNCTION,
    KEYWORD_VOID

}KeyWord;

typedef enum
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_ID,
    TYPE_FUNID,
    TYPE_KEYWORD,
    TYPE_EOF,
    TYPE_DECLARE_ST,    // declare strict types
    TYPE_QMARK,         // for ?int var
    
    //Operators
    TYPE_ADD,           // +
    TYPE_SUB,           // -
    TYPE_MUL,           // *
    TYPE_DIV,           // /
    TYPE_MOD,           // %
    TYPE_ASSIGN,        // =
    TYPE_EQTYPES,       // ===    
    TYPE_NOTEQTYPES,    // !==   
    TYPE_LESS,          // <
    TYPE_GREATER,       // >
    TYPE_LESSEQ,        // <=
    TYPE_GREATEREQ,     // >=
    TYPE_CONCAT,        // .

    //Special symbols
    TYPE_DOLLAR,        // $
    TYPE_AMPERSAND,     // &
    TYPE_NEG,           // !
    TYPE_SEMICOLON,     // ;
    TYPE_BEGIN,         // <?php
    TYPE_END            // ?>

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
}Token;

//TODO pridat deklarace funkce/funkci

#endif