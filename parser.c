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

void freeAll(Symtable *globalST)
{
    ST_freeTable(globalST);
}

STItem *serveSymTable(Symtable *table, Token token)
{
    STItem *item = ST_searchTable(table, DS_string(token.attribute.dString));
    if (item == NULL)
    {
        if (token.type == TYPE_ID)
        {
            STItemData data;
            data.varData.VarType = 'i';
            ST_insertItem(table, DS_string(token.attribute.dString), ST_ITEM_TYPE_VARIABLE, data);
        }

    }else
    { // item found in ST
        printf("Incoming item is already in globalST: %s\n",item->key);
    }

    return item;
}

int checkProlog(Token *tokenArr) //remove tokenArr SIMULATION
{
    Token token = getTokenSim(tokenArr);
    if (token.type != TYPE_BEGIN)
    {
        fprintf(stderr, "Expected <?php at beggining of the file!\n");
        return (ERR_SYN);
    }

    token = getTokenSim(tokenArr); // simulation
    while(token.type == TYPE_COMM){
        token = getTokenSim(tokenArr);
    }

    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        return (ERR_SYN);
    }
    return 0;
}

int parser(Token *tokenArr) // getToken();
{
    int err;
    // check prolog
    err = checkProlog(tokenArr); // remove tokenArr SIMULATION
    if (err != 0){
        return err;
    }

    Token token = getTokenSim(tokenArr); // simulation
    Symtable *globalST = ST_initTable(10);

    // while tokens available
    while (1)
    {
        if (token.type == TYPE_FUNID || token.type == TYPE_ID || token.type == TYPE_LABEL)
        {
            serveSymTable(globalST, token);
        }

        // epilog check
        if (token.type == TYPE_END)
        {
            token = getTokenSim(tokenArr);
            if (token.type != TYPE_EOF)
            {
                fprintf(stderr, "Expected EOF after \"?>\"!\n");
                freeAll(globalST);
                return(ERR_SYN);
            }
            break;
        }

        if (token.type == TYPE_FUNID || token.type == TYPE_ID || token.type == TYPE_LABEL)
        {
            DS_dispose(token.attribute.dString);
        }

        token = getTokenSim(tokenArr);
    } // while (token.type != TYPE_EOF)

    freeAll(globalST);
    //printf("Parser OK!\n");
    return 0;
}
