
#ifndef _IFJ_CG
#define _IFJ_CG

#include "stack.h"
#include <string.h>

int generateBuiltInFunc(DynamicString *);
int callFunc();
int CODEpushValue(DynamicString *, Token);
#endif