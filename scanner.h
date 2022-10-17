#include "dynamicString.h"

typedef enum {
    KEYWORD_IF,
    KEYWORD_WHILE

}KeyWord;

typedef enum
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_ID,
    TYPE_KEYWORD,
    TYPE_EOF,


    // oparators
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_MOD,
    TYPE_EQ

} TokenType;

typedef union
{
    KeyWord keyword;
    int intV;
    double doubleV;
    DynamicString *dString;
    
}TokenAtribute;

typedef struct
{
    TokenType type;
    TokenAtribute atribute;
}Token;