
#ifndef _IFJ_CG
#define _IFJ_CG

#include "stack.h"
#include <string.h>

int generateBuiltInFunc(DynamicString *);
int callFunc();
int CODEgenerateFuncCall(DynamicString *,Token,int);
int CODEassign(DynamicString *, Token);
int CODEdefVar(DynamicString *dString, Token token);
int CODEpushValue(DynamicString *, Token);
int CODEifStart(DynamicString *, int);
int CODEelse(DynamicString *, int);
int CODEendIf(DynamicString *, int);
int CODEconvert2Type(DynamicString *, char *, char);
#endif