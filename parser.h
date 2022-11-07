/**
 * @file parser.h
 * @author Jakub Mašek
 * @brief parser header file
 * @version 0.1
 * @date 27-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _IFJ_PARSER
#define _IFJ_PARSER

#include "getTokenSim.h" //simulation DELETE ME
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "stack.h"

int parser(Token *);
void freeAll(Symtable *);
int checkProlog(Token *);
STItem *serveSymTable(Symtable *, Token);

#endif