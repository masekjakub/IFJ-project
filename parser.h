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
#include "codeGenerator.h"

// rules funcs
ErrorType exprAnal();
ErrorType ruleProg();
ErrorType ruleStatList();
ErrorType ruleId();
ErrorType ruleStat();
ErrorType ruleFuncdef();
ErrorType ruleAssign();
ErrorType ruleParam();
ErrorType ruleParams();
ErrorType ruleParams2();
ErrorType ruleArgs();
ErrorType ruleArgs2();
ErrorType ruleReturn();

// to use in rule functions
Token newToken(int);
void freeAll(Symtable *);
Symtable *getTable(int);
void makeError();

// other
int parser(Token *);

#endif