/**
 * @file symtable.h
 * @author Martin Zelenák
 * @brief Header file of symtable.c
 * @version 1.0
 * @date 2022-10-15
 */

#ifndef _IFJ_SYMTABLE
#define _IFJ_SYMTABLE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

//Data of variable type symtable item
typedef struct{
    //Data type of variable ('i','f',...)
    char VarType;
    //Index of variable
    unsigned int VarIndex;
}STVarData;

//Data of function type symtable item
typedef struct{
    //1st char represents return type
    //other chars represent types of params
    //("vii","ifi",...)
    //capital chars represent 1 type or null(?INT -> 'I')
    //'x' represents any type
    char *funTypes;
    bool defined;
}STFunData;

//Data of symtable items of different types
typedef union{
    //Variable
    STVarData varData;
    //Function
    STFunData funData;
}STItemData;

//Types of symtable items
typedef enum{
    ST_ITEM_TYPE_VARIABLE,
    ST_ITEM_TYPE_FUNCTION
}STItemType;

//Item of symtable
typedef struct symtableItem{
    //Name of variable/function
    char *key;
    //Variable or function
    STItemType type;
    STItemData data;
    //Ptr to next item mapped to the same index
    struct symtableItem *nextItem;
}STItem;

//Table of symbols (hash table)
typedef struct symtable{
    //Array of ptrs to first items of each index
    STItem **items;
    //Number of indicies
    unsigned int size;
    //Number of items
    unsigned int count;
    //Current variable index
    unsigned int curVarIndex;
}Symtable;


unsigned int ST_hashFunction(char *, unsigned int);

Symtable *ST_initTable(unsigned int);

STItem *ST_initItem(char *, STItemType, STItemData);

void ST_freeItem(STItem *);

void ST_freeItemList(STItem *);

void ST_freeTable(Symtable *);

void ST_expand(Symtable *);

void ST_shrink(Symtable *);

STItem *ST_searchTable(Symtable *, char *);

STItem *ST_insertItem(Symtable *, char *, STItemType, STItemData);

STItem *ST_updateVarType(Symtable *, char *, char);

void ST_removeItem(Symtable *, char *);

#endif