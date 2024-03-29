/**
 * @file codeGenerator.h
 * @authors Martin Zelenák, Jakub Mašek, Vojta Kuchař
 * @brief Header file for codeGenerator.c
 * @version 1.0
 * @date 2022-11-06
 */

#ifndef _IFJ_CG
#define _IFJ_CG

#include "parser.h"
#include <string.h>

void CODEmain(DynamicString *);
void CODEbuiltInFunc(DynamicString *);
void CODEfuncDef(DynamicString *, char *);
void CODEparam(DynamicString *, char *);
void CODEpopParam(DynamicString *, char *, char);
void CODEfuncReturn(DynamicString *, char, int);
void CODEfuncDefEnd(DynamicString *, char *, bool);
void CODEfuncCall(DynamicString *,Token,int);
void CODEcallWrite(DynamicString *, int);
void CODEassign(DynamicString *, Token);
void CODEcheckInitVar(DynamicString *, char *, bool, int);
void CODEdefVar(DynamicString **, char *, int);
void CODEpopValue(DynamicString *, char *, bool);
void CODEpushValue(DynamicString *, Token);
void CODEifStart(DynamicString **, int, int);
void CODEelse(DynamicString *, int);
void CODEifEnd(DynamicString *, int);
void CODEwhileStart(DynamicString **, int, int);
void CODEwhileCond(DynamicString *, int);
void CODEwhileEnd(DynamicString *, int);
void CODEconvert2Type(DynamicString *, char *, char);
void CODEarithmetic(DynamicString *, int, Token *);
#endif