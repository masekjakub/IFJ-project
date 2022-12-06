/**
 * @file parser.h
 * @author Jakub Mašek, Martin Zelenák
 * @brief parser header file
 * @version 0.1
 * @date 2022-10-16
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


//Struct type representing final string of generated code
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


// Functions representing LL-Grammar rules
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

// Utility functions
Token newToken(int);
DynamicString *getCode(int);
DynamicString **getCodePtr(int);
Code *getCodeStruct(int);
Symtable *getTable(int);
void makeError();

// Main parser function
#ifndef scanner
int parser(Token *);
#else
int parser();
#endif

#endif