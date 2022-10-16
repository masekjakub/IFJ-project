/**
 * @file symtable.c
 * @author Martin Zelenák
 * @brief Implementaion of ST as Hash Table
 * @version 1.0
 * @date 2022-10-15
 */

#include "symtable.h"

/**
 * @brief Function for mapping key to a ST index
 * 
 * @param key String used for calculating the index 
 * @param tableSize Size of the symbol table
 * @return Index in ST the key maps to
 */
unsigned int ST_hashFunction(char *key, int tableSize){
    unsigned int index = 0;
    for(int i = 0; key[i] != '\0'; i++){
        index += key[i];
    }
    return index % tableSize;
}

/**
 * @brief Allocates and initializes a new ST item
 * 
 * @param key Key of created item
 * @param type Type of created item (variable/function/label)
 * @param data Data of the correct item type
 * @return Ptr to the created item
 */
STItem *ST_initItem(char *key, STItemType type, STItemData data){
    STItem *item = (STItem *)malloc(sizeof(STItem));
    if(item == NULL){
        exit(99);        //TODO Replace with defined error
    }
    item->key = key;
    item->type = type;
    item->data = data;
    item->nextItem = NULL;
    return item;
}

/**
 * @brief Allocates and initializes a new symbol table
 * 
 * @param size Size of ST / Number of indicies in ST
 * @return Ptr to the created table
 */
Symtable *ST_initTable(unsigned int size){
    Symtable *table = (Symtable *)malloc(sizeof(Symtable));
    if(table == NULL){
        exit(99);        //TODO Replace with defined error
    }
    table->size = size;
    table->items = (STItem **)calloc(size, sizeof(STItem *));
    for(unsigned int i = 0; i < size; i++){
        table->items[i] = NULL;
    }
    return table;
}

/**
 * @brief Frees allocated memory for one item
 * 
 * @param item Ptr to the item to be freed
 */
void ST_freeItem(STItem *item){
    free(item->key);
    if(item->type == ST_ITEM_TYPE_FUNCTION){
        free(item->data.funData.funTypes);
    }
    free(item);
}

/**
 * @brief Frees allocated memory for one item and all the items it points to
 * 
 * @param firstItem Item at the start of the list to be freed
 */
void ST_freeItemList(STItem *firstItem){
    STItem *curItem = firstItem;
    STItem *nextItem = NULL;
    while(curItem != NULL){
        nextItem = curItem->nextItem;
        ST_freeItem(curItem);
        curItem = nextItem;
    }
}

/**
 * @brief Frees the allocated memory for ST and all it's items
 * 
 * @param table Ptr to the table to be freed
 */
void ST_freeTable(Symtable *table){
    for(unsigned int i = 0; i < table->size; i++){
        if(table->items[i] != NULL){
            ST_freeItemList(table->items[i]);
        }
    }
    free(table->items);
    free(table);
}

/**
 * @brief Searches for an item in ST with a specific key
 * 
 * @param table Ptr to the ST to search
 * @param key Key by which to search
 * @return Ptr to the found item | NULL if not found
 */
STItem *ST_searchTable(Symtable *table, char *key){
    unsigned int index = ST_hashFunction(key, table);
    if(table->items[index] == NULL){
        return NULL;
    }
    STItem *curItem = table->items[index];
    while(!strcmp(curItem->key,key)){
        curItem = curItem->nextItem;
        if(curItem == NULL) return NULL;
    }
    return curItem;
}

/**
 * @brief Initializes and inserts new item into ST with given params
 * 
 * @param table ST to insert into
 * @param key Key of the new item
 * @param type Type of the new item
 * @param data Correct data for the given type of new item
 */
void ST_insertItem(Symtable* table, char* key, STItemType type, STItemData data){
    unsigned int index = ST_hashFunction(key, table->size);
    STItem *newItem = ST_initializeItem(key, type, data);
    if(table->items[index] == NULL){
        table->items[index] = newItem;
    }else{
        STItem *curItem = table->items[index];
        while(curItem->nextItem != NULL){
            curItem = curItem->nextItem;
        }
        curItem->nextItem = newItem;
    }
}

/**
 * @brief Removes an item from ST
 * 
 * @param table ST to remove from
 * @param key Key of the item to be removed
 */
void ST_removeItem(Symtable *table, char *key){
    //Search for item
    unsigned int index = ST_hashFunction(key, table);
    if(table->items[index] == NULL){
        return;
    }
    STItem *curItem = table->items[index];
    STItem *prevItem = NULL;
    while(strcmp(curItem->key,key)){
        prevItem = curItem;
        curItem = curItem->nextItem;
        if(curItem == NULL) return;
    }
    //Connect items after deleted item (beforeDelItem -> AfterDelItem)
    prevItem->nextItem = curItem->nextItem;
    //Free deleted item
    ST_freeItem(curItem);
}
