#ifndef TOKENSIM
#define TOKENSIM

#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void makeToken(Token*, TokenType, KeyWord, int, double, char *, int);
Token *defineTokens(Token *);
Token *initTokens();
Token getTokenSim();

#endif