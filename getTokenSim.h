/**
 * @file getTokenSim.h
 * @author Jakub Mašek
 * @brief header file for getTokenSim.c
 * @version 0.1
 * @date 16-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _IFJ_TOKENSIM
#define _IFJ_TOKENSIM

#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void makeToken(Token*, TokenType, KeyWord, int, double, char *, int);
Token *defineTokens(Token *);
Token *initTokens();
Token getTokenSim();

#endif