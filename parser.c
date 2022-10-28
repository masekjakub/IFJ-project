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
#include "parser.h"

void serveSymTable(Symtable *table, Token token){
    if (token.type == TYPE_ID){
        STItemData data;
        data.varData.VarType = 'i';
        ST_insertItem(table, token.attribute.dString->string, ST_ITEM_TYPE_VARIABLE, data);
    }
}

int main()
{
    Token *tokenArr = initTokens(); // simulation
    
    // check prolog
    Token token = getTokenSim(tokenArr);//simulation
    if (token.type != TYPE_BEGIN)
    {
        fprintf(stderr,"Expected <?php at beggining of the file!\n");
        exit (ERR_SYN);
    }
    token = getTokenSim(tokenArr); // simulation
    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        exit (ERR_SYN);
    }

    Symtable *globalST = ST_initTable(10);

    //while tokens available
    while (token.type != TYPE_EOF)
    {
        if (token.type == TYPE_FUNID || token.type == TYPE_ID){
            serveSymTable(globalST,token);
        }
        if (token.type == TYPE_END){
            token = getTokenSim(tokenArr);
            if(token.type != TYPE_EOF){
                fprintf(stderr, "Expected EOF after ?>!\n");
                exit(ERR_SYN);
            }
            break;
        }
        token = getTokenSim(tokenArr);
    }

    //epilog

    STItem *item = ST_searchTable(globalST, "var");
    if(item == NULL){
        printf("NULL");
        return 1;
    }
        printf("ID: %s\n", item->key);

    return 0;
}
