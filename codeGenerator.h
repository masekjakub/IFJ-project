
#ifndef _IFJ_CG
#define _IFJ_CG

#include "stack.h"
#include "parser.h"
#include <string.h>

void CODEmain(DynamicString *);
int CODEbuiltInFunc(DynamicString *);
int callFunc();
int CODEfuncDef(DynamicString *, char *);
int CODEparam(DynamicString *, char *);
int CODEpopParam(DynamicString *, char *, char);
int CODEfuncReturn(DynamicString *, char, int);
int CODEfuncDefEnd(DynamicString *, char *, bool);
int CODEfuncCall(DynamicString *,Token,int);
int CODEcallWrite(DynamicString *, int);
int CODEassign(DynamicString *, Token);
int CODEcheckInitVar(DynamicString *, char *, bool, int);
int CODEdefVar(DynamicString **, char *, int);
int CODEpopValue(DynamicString *, char *, bool);
int CODEpushValue(DynamicString *, Token);
int CODEifStart(DynamicString **, int, int);
int CODEelse(DynamicString *, int);
int CODEifEnd(DynamicString *, int);
int CODEwhileStart(DynamicString **, int, int);
int CODEwhileCond(DynamicString *, int);
int CODEwhileEnd(DynamicString *, int);
int CODEconvert2Type(DynamicString *, char *, char);
void CODEarithmetic(int ruleUsed, Token *tokenArr, Token endToken, int isGlobal);
#endif