#include "dynamicString.h"

typedef enum {
    KEYWORD_IF,

}KeyWord;

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,

}TokenType;

typedef union
{
    KeyWord keyword;
    int intV;
    double doubleV;
    DynamicString *string;
    
}TokenAtribute;

typedef union
{
    TokenType type;
    TokenAtribute atribute;
}Token;