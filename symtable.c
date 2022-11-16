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
 * @param tableSize Size of the ST
 * @return Index in ST the key maps to
 */
unsigned int ST_hashFunction(char *key, unsigned int tableSize){
    unsigned int index = 0;
    for(int i = 0; key[i] != '\0'; i++){
        index += key[i];
    }
    return (index % tableSize);
}

/**
 * @brief Allocates and initializes a new ST item
 * 
 * @param key Key of created item (contents of string are coppied)
 * @param type Type of created item (variable/function)
 * @param data Data of the correct item type
 * @return Ptr to the created item
 */
STItem *ST_initItem(char *key, STItemType type, STItemData data){
    STItem *item = (STItem *)malloc(sizeof(STItem));
    if(item == NULL){
        exit(ERR_INTERN);
    }
    item->key = (char *)malloc((strlen(key)+1) * sizeof(char));
    strcpy(item->key, key);
    item->type = type;
    item->data = data;
    if(type == ST_ITEM_TYPE_FUNCTION){
        item->data.funData.funTypes = malloc((strlen(data.funData.funTypes)+1) * sizeof(char));
        strcpy(item->data.funData.funTypes, data.funData.funTypes);
    }
    item->nextItem = NULL;
    return item;
}

/**
 * @brief Allocates and initializes a new symbol table
 * 
 * @param size Size of ST / Number of indicies in ST !!! Preferably a power of 2 !!!
 * @return Ptr to the created table
 */
Symtable *ST_initTable(unsigned int size){
    Symtable *table = (Symtable *)malloc(sizeof(Symtable));
    if(table == NULL){
        exit(ERR_INTERN);
    }
    table->size = size;
    table->curVarIndex = 0;
    table->items = (STItem **)calloc(size, sizeof(STItem *));
    if(table->items == NULL){
        exit(ERR_INTERN);
    }
    for(unsigned int i = 0; i < size; i++){
        table->items[i] = NULL;
    }
    return table;
}

/**
 * @brief Frees allocated memory for one item
 * 
 * @param item Item to be freed
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
 * @param table ST to be freed
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
 * @brief Resizes ST to a bigger well chosen size
 * 
 * @param table ST to resize
 */
void ST_expand(Symtable *table){
    //EXPANDING
    unsigned int newSize = table->size * 2; //TODO Expand size to the next prime number
    table->items = (STItem **)realloc(table->items, newSize * sizeof(STItem *));
    if(table->items == NULL){
        exit(ERR_INTERN);
    }
    unsigned int oldSize = table->size;
    table->size = newSize;

    //REHASHING
    STItem *rehashItem = NULL;  //Item being rehashed
    STItem *prevItem = NULL;    //Previous item to rehashItem in item list on cur. index
    STItem *curItem = NULL;     //Cursor for going through item lists
    unsigned int newIndex;
    for(unsigned int i = 0; i < oldSize; i++){
        //Check for empty index
        if(table->items[i] == NULL) continue;

        //REHASHING THE FIRST item on cur. index (while the new first item needs to be moved)
        rehashItem = table->items[i];
        newIndex = ST_hashFunction(rehashItem->key, table->size);
        while(newIndex != i){
            //POP rehashed item
            table->items[i] = rehashItem->nextItem;
            rehashItem->nextItem = NULL;
            
            //REINSERT it
            if(table->items[newIndex] == NULL){
                table->items[newIndex] = rehashItem;
            }else{
                curItem = table->items[newIndex];
                while(curItem->nextItem != NULL){
                    curItem = curItem->nextItem;
                }
                curItem->nextItem = rehashItem;
            }

            //NEXT CYCLE
            rehashItem = table->items[i];
            if(rehashItem == NULL) break;
            newIndex = ST_hashFunction(rehashItem->key, table->size);
        }
        if(rehashItem == NULL) continue;    //If list was emptied

        //REHASHING THE REST of the items in list on cur. index
        prevItem = rehashItem;              //First item (doesn't need to be moved)
        rehashItem = rehashItem->nextItem;  //Second item (rehashing)
        while(rehashItem != NULL){
            newIndex = ST_hashFunction(rehashItem->key, table->size);
            if(newIndex != i){
                //POP rehashed item
                prevItem->nextItem = rehashItem->nextItem;
                rehashItem->nextItem = NULL;

                //REINSERT it
                if(table->items[newIndex] == NULL){
                    table->items[newIndex] = rehashItem;
                }else{
                    curItem = table->items[newIndex];
                    while(curItem->nextItem != NULL){
                        curItem = curItem->nextItem;
                    }
                    curItem->nextItem = rehashItem;
                }
                //NEXT CYCLE (if moving)
                rehashItem = prevItem->nextItem;
                continue;
            }
            //NEXT CYCLE (if not moving)
            prevItem = rehashItem;
            rehashItem = rehashItem->nextItem;
        }
    }
}

/**
 * @brief Resizes ST to a smaller well chosen size
 * 
 * @param table ST to resize
 */
void ST_shrink(Symtable *table){
    //REHASHING
    unsigned int newSize = table->size / 2;
    STItem *rehashItem = NULL;  //Item being rehashed
    STItem *prevItem = NULL;    //Previous item to rehashItem in item list on cur. index
    STItem *curItem = NULL;     //Cursor for going through item lists
    unsigned int newIndex;
    for(unsigned int i = 0; i < table->size; i++){
        //Check for empty index
        if(table->items[i] == NULL) continue;

        //REHASHING THE FIRST item on cur. index (while the new first item needs to be moved)
        rehashItem = table->items[i];
        newIndex = ST_hashFunction(rehashItem->key, newSize);
        while(newIndex != i){
            //POP rehashed item
            table->items[i] = rehashItem->nextItem;
            rehashItem->nextItem = NULL;
            
            //REINSERT it
            if(table->items[newIndex] == NULL){
                table->items[newIndex] = rehashItem;
            }else{
                curItem = table->items[newIndex];
                while(curItem->nextItem != NULL){
                    curItem = curItem->nextItem;
                }
                curItem->nextItem = rehashItem;
            }

            //NEXT CYCLE
            rehashItem = table->items[i];
            if(rehashItem == NULL) break;
            newIndex = ST_hashFunction(rehashItem->key, newSize);
        }
        if(rehashItem == NULL) continue;    //If list was emptied

        //REHASHING THE REST of the items in list on cur. index
        prevItem = rehashItem;              //First item (doesn't need to be moved)
        rehashItem = rehashItem->nextItem;  //Second item (rehashing)
        while(rehashItem != NULL){
            newIndex = ST_hashFunction(rehashItem->key, newSize);
            if(newIndex != i){
                //POP rehashed item
                prevItem->nextItem = rehashItem->nextItem;
                rehashItem->nextItem = NULL;

                //REINSERT it
                if(table->items[newIndex] == NULL){
                    table->items[newIndex] = rehashItem;
                }else{
                    curItem = table->items[newIndex];
                    while(curItem->nextItem != NULL){
                        curItem = curItem->nextItem;
                    }
                    curItem->nextItem = rehashItem;
                }
                //NEXT CYCLE (if moving)
                rehashItem = prevItem->nextItem;
                continue;
            }
            //NEXT CYCLE (if not moving)
            prevItem = rehashItem;
            rehashItem = rehashItem->nextItem;
        }
    }

    table->items = (STItem **)realloc(table->items, newSize * sizeof(STItem *));
    if(table->items == NULL){
        exit(ERR_INTERN);
    }
    table->size = newSize;
}

/**
 * @brief Searches for an item in ST with a specific key
 * 
 * @param table ST to search in
 * @param key Key by which to search
 * @return Ptr to the found item | NULL if not found
 */
STItem *ST_searchTable(Symtable *table, char *key){
    unsigned int index = ST_hashFunction(key, table->size);
    if(table->items[index] == NULL){
        return NULL;
    }
    STItem *curItem = table->items[index];
    while(strcmp(curItem->key,key)){
        curItem = curItem->nextItem;
        if(curItem == NULL) return NULL;
    }
    return curItem;
}

/**
 * @brief Initializes and inserts new item into ST with given params
 * or updates an existing one
 * 
 * @param table ST to insert into
 * @param key Key of the new item
 * @param type Type of the new item
 * @param data Correct data for the given type of new item
 * @return Ptr to the new item in ST
 */
STItem *ST_insertItem(Symtable* table, char* key, STItemType type, STItemData data){
    unsigned int index = ST_hashFunction(key, table->size);
    STItem *newItem = ST_initItem(key, type, data);
    if(table->items[index] == NULL){
        table->items[index] = newItem;
        //Setting VarIndex if new item is a variable
        if(type == ST_ITEM_TYPE_VARIABLE){
            newItem->data.varData.VarIndex = table->curVarIndex;
            table->curVarIndex++;
        }
    }else{
        //Going through item list
        //searching for end of list / checking if item already exists
        STItem *curItem = table->items[index];
        STItem *prevItem = NULL;
        bool found = false;
        while(curItem != NULL){
            //If item already exists
            if(!strcmp(curItem->key, key)){
                //Linking to previous item
                if(prevItem != NULL){
                    prevItem->nextItem = newItem;
                }else{
                    table->items[index] = newItem;
                }
                newItem->nextItem = curItem->nextItem;
                //Setting VarIndex if new item is a variable
                if(type == ST_ITEM_TYPE_VARIABLE){
                    if(curItem->type == ST_ITEM_TYPE_VARIABLE){
                        newItem->data.varData.VarIndex = curItem->data.varData.VarIndex;
                    }else{
                        newItem->data.varData.VarIndex = table->curVarIndex;
                        table->curVarIndex++;
                    }
                }
                ST_freeItem(curItem);
                found = true;
                break;
            }
            prevItem = curItem;
            curItem = curItem->nextItem;
        }
        //If item didn't exist before
        if(!found){
            prevItem->nextItem = newItem;
            if(type == ST_ITEM_TYPE_VARIABLE){
                newItem->data.varData.VarIndex = table->curVarIndex;
                table->curVarIndex++;
            }
        }
    }
    
    table->count++;
    if(table->count / table->size >= 3){
        ST_expand(table);
    }
    return newItem;
}

STItem *ST_updateVarType(Symtable *table, char *key, char newType){
    STItem *updatedItem = ST_searchTable(table, key);
    if(updatedItem == NULL){
        return NULL;
    }
    if(updatedItem->type == ST_ITEM_TYPE_FUNCTION){
        return NULL;
    }
    updatedItem->data.varData.VarType = newType;
    return updatedItem;
}

/**
 * @brief Removes an item from ST and frees it
 * 
 * @param table ST to remove from
 * @param key Key of the item to be removed and freed
 */
void ST_removeItem(Symtable *table, char *key){
    //Get item's index
    unsigned int index = ST_hashFunction(key, table->size);
    if(table->items[index] == NULL){
        return;
    }

    //Removement if item is first
    STItem *curItem = table->items[index];
    if(!strcmp(curItem->key, key)){
        table->items[index] = curItem->nextItem;
        
        ST_freeItem(curItem);
        table->count--;
        if((double)(table->count) / (double)(table->size) <= 0.25){
            ST_shrink(table);
        }
        return;
    }

    //Removement if item is within list
    STItem *prevItem = curItem;
    curItem = curItem->nextItem;
    while(strcmp(curItem->key,key)){
        prevItem = curItem;
        curItem = curItem->nextItem;
        if(curItem == NULL) return;
    }
    //Connect items after deleted item (beforeDelItem -> AfterDelItem)
    prevItem->nextItem = curItem->nextItem;
    //Free deleted item
    ST_freeItem(curItem);

    table->count--;
    if((double)(table->count) / (double)(table->size) <= 0.25){
        ST_shrink(table);
    }
}
