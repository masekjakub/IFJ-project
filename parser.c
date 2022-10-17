/**
 * @file parser.c
 * @author Jakub Mašek
 * @brief parser for IFJ22 translator
 * @version 0.1
 * @date 16-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "getTokenSim.h" //simulation DELETE ME
//#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

void serveSymTable(Symtable *table, Token token){
    if (token.type == TYPE_ID){
        STItemData data;
        data.varData.VarType = 'i';
        ST_insertItem(table, token.atribute.dString->string, ST_ITEM_TYPE_VARIABLE, data);
    }
}

int main()
{
    Token *tokenArr = initTokens();

    Token token = getTokenSim(tokenArr);
    Symtable *globalST = ST_initTable(10);
    while(token.type != TYPE_EOF){
        printf("%d\n",token.type);
        serveSymTable(globalST,token);
        token = getTokenSim(tokenArr);

    }
    STItem *item = ST_searchTable(globalST, "var");
    if(item == NULL){
        printf("NULL");
        return 1;
    }
        printf("ID: %s\n", item->key);

    return 0;
}
