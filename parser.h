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
//////////////////////////
#define scanner // use scanner
//////////////////////////

#ifndef _IFJ_PARSER
#define _IFJ_PARSER

#include "getTokenSim.h" //simulation DELETE ME
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "stack.h"
#include "dynamicString.h"
#include "codeGenerator.h"

typedef enum
{
    R, // >
    L, // <
    E, // =
    N  // none
} precTableSymbols;

typedef struct
{
    //String of generated code
    DynamicString *string;
    //Index of line in code string
    //-Last line of code, that is always performed(unconditioned)
    //-Index of first char after last unconditioned line
    //- -1 if current line is unconditioned
    int lastUnconditionedLine;
} Code;


// rules funcs
ErrorType exprAnal();
ErrorType ruleProg();
ErrorType ruleStatList(bool);
ErrorType ruleStat();
ErrorType ruleId();
ErrorType ruleFuncdef();
ErrorType ruleAssign();
ErrorType ruleParams();
ErrorType ruleParams2(Stack *);
ErrorType ruleParam(Stack *);
ErrorType ruleArgs();
ErrorType ruleArgs2();
ErrorType ruleReturn();
ErrorType functionCallCheckAndProcess();

// to use in rule functions
Token newToken(int);
DynamicString *getCode(int);
DynamicString **getCodePtr(int);
Code *getCodeStruct(int);
Symtable *getTable(int);
void makeError();

// other
#ifndef scanner
int parser(Token *);
#else
int parser();
#endif

#endif