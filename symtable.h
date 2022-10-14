#include "dynamicString.h"

typedef struct symtable{
    SymtableItem **items;
    int size;
    int count;
}Symtable;

typedef struct item{
    char *key;
    SymtableItemData data;
}SymtableItem;

typedef struct data{
    //Variable
    char VarType;
    int VarPosition;
    //Function
    char *funTypes;
    bool defined;
}SymtableItemData;

unsigned long hashFunction(char *str);