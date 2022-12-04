
#ifndef _IFJ_CG
#define _IFJ_CG

#include "stack.h"
#include <string.h>

void CODEmain(DynamicString *);
int CODEbuiltInFunc(DynamicString *);
int callFunc();
int CODEfuncDef(DynamicString *, char *);
int CODEparam(DynamicString *, char *);
int CODEfuncReturn(DynamicString *, char, int);
int CODEfuncDefEnd(DynamicString *, char *, bool);
int CODEfuncCall(DynamicString *,Token,int);
int CODEcallWrite(DynamicString *, int);
int CODEassign(DynamicString *, Token);
int CODEcheckInitVar(DynamicString *, char *, bool, int);
int CODEdefVar(DynamicString **, Token, int);
int CODEpopValue(DynamicString *, char *, bool);
int CODEpushValue(DynamicString *, Token);
int CODEifStart(DynamicString *, int);
int CODEelse(DynamicString *, int);
int CODEendIf(DynamicString *, int);
int CODEconvert2Type(DynamicString *, char *, char);
#endif