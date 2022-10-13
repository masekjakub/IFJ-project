#include "dynamicString.h"

typedef enum {
    KEYWORD_IF,
    KEYWORD_EQ

}KeyWord;

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_ID,
    TYPE_OPERATION,
    TYPE_KEYWORD

}TokenType;

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