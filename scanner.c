/**
 * @file scanner.c
 * @author Filip Polomski
 * @brief scanner for IFJ22 translator
 * @version 0.1
 * @date 27-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "scanner.h"

FILE *source;

int rowNumber = 1;

/**
 * @brief Set the Source File object
 * 
 * @param f 
 */
void setSourceFile(FILE *f)
{
  source = f;
}

/**
 * @brief Function for checking if its keyword or name of function
 * 
 * @param dynamicString 
 * @param token 
 * @return true, false
 */
bool isKeyword(DynamicString *dynamicString, Token *token){
    if (!strcmp(DS_string(dynamicString), "if")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_IF;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "else")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_ELSE;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "while")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_WHILE;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "float")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_FLOAT;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "string")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_STRING;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "null")){
        token->type = TYPE_NULL;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "function")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_FUNCTION;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "void")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_VOID;
        return true;
    }
    if (!strcmp(DS_string(dynamicString), "return")){
        token->type = TYPE_KEYWORD;
        token->attribute.keyword = KEYWORD_RETURN;
        return true;
    }
    return false;
}

/**
 * @brief Function for getting next token
 * 
 * @return Token 
 */
Token getToken(){ 
    int c;
    int wasDot = 0;
    int wasE = 0;
    State state = STATE_START;
    DynamicString *dynamicString = DS_init();
    while (1){
        Token token;
        c = getc(source);
        if (c == '\n'){
            rowNumber++;
        }
        token.rowNumber = rowNumber;
        switch (state){
            // Starting state of finite automat
            case STATE_START:
                if (isspace(c)){
                    state = STATE_START;
                    break;
                }
                // DIV and commentary
                else if (c == '/'){
                    c = getc(source);
                    // Check for commentary in line
                    if (c == '/'){
                        while (c != '\n' && c != EOF){ c = getc(source); }
                        ungetc(c, source);
                        token.type = TYPE_COMM;
                        return token;
                    }
                    // Check for block commentary
                    else if (c == '*'){ 
                        c = getc(source);
                        while (1){
                            if (c == '\n'){
                                rowNumber++;
                                token.rowNumber = rowNumber;
                            }
                            if (c == '*'){
                                if ((c = getc(source)) == '/'){
                                    token.type = TYPE_COMM;
                                    return token;
                                }
                                ungetc(c, source);
                            }
                            else if (c == EOF){
                                fprintf(stderr, "Expected end of comment: \"*/\"!\n");
                                token.type = TYPE_LEXERR;
                                return token;
                            }
                            c = getc(source);
                        }
                    }
                    // Check for div
                    ungetc(c, source);
                    token.type = TYPE_DIV;
                    return token;
                }
                // EOF
                else if (c == EOF){
                    token.type = TYPE_EOF;
                    return token;
                }
                // MODULO
                else if (c == '%'){
                    token.type = TYPE_MOD;
                    return token;
                }
                // MULTIPLICATION
                else if (c == '*'){
                    token.type = TYPE_MUL;
                    return token;
                }
                // ADDITION
                else if (c == '+'){
                    token.type = TYPE_ADD;
                    return token;
                }
                // CONCATENATION
                else if (c == '.'){
                    token.type = TYPE_CONCAT;
                    return token;
                }
                // SEMICOLON
                else if (c == ';'){
                    token.type = TYPE_SEMICOLON;
                    return token;
                }
                // LEFT BRACKET
                else if (c == '('){
                    token.type = TYPE_LBRACKET;
                    return token;
                }
                // RIGHT BRACKET
                else if (c == ')'){
                    token.type = TYPE_RBRACKET;
                    return token;
                }
                // LEFT BRACES
                else if (c == '{'){
                    token.type = TYPE_LBRACES;
                    return token;
                }
                // RIGHT BRACES
                else if (c == '}'){
                    token.type = TYPE_RBRACES;
                    return token;
                }
                // COMMA
                else if (c == ','){
                    token.type = TYPE_COMMA;
                    return token;
                }
                // COLON
                else if (c == ':'){
                    token.type = TYPE_COLON;
                    return token;
                }
                // SUBTRACTION
                else if (c == '-'){
                    token.type = TYPE_SUB;
                    return token;
                }
                // Checks if char is number, then go to next state -> STATE_INTEGER
                else if (isdigit(c)){
                    state = STATE_INTEGER;
                    ungetc(c, source);
                    break;
                }
                // ?, ?>
                else if (c == '?'){
                    if ((c = getc(source)) == '>'){
                        token.type = TYPE_END;
                        return token;
                    }
                    fprintf(stderr, "Wrong epilog!\nExpected epilog: \"?>\" on line %d!\n", token.rowNumber);
                    token.type = TYPE_LEXERR;
                    return token;
                }
                // =, ===
                else if (c == '='){
                    // Checks if token is type === or invalid type ==
                    c = getc(source);
                    if (c == '='){
                        if ((c = getc(source)) == '='){
                            token.type = TYPE_EQTYPES;
                            return token;
                        }
                        fprintf(stderr, "Non-existing operator \"==\" on line %d!\nThere are only operands: \"=\" or \"===\".\n", token.rowNumber);
                        token.type = TYPE_LEXERR;
                        return token;
                    }
                    ungetc(c, source);
                    // If token is type =, return it
                    token.type = TYPE_ASSIGN;
                    return token;
                }
                // !=, !==
                else if (c == '!'){
                    // Checks if token is type !== or invalid type !=
                    c = getc(source);
                    if (c == '='){
                        if ((c = getc(source)) == '='){
                            token.type = TYPE_NOTEQTYPES;
                            return token;
                        }
                        fprintf(stderr, "Non-existing operator \"!=\" on line %d!\nThere are only operands: \"!\" or \"!==\".\n", token.rowNumber);
                        token.type = TYPE_LEXERR;
                        return token;
                    }
                    ungetc(c, source);
                    // If token is type !, return it
                    token.type = TYPE_NEG;
                    return token;
                }
                // <, <=, <?php
                else if (c == '<'){
                    // Checks if token is type <=
                    if ((c = getc(source)) == '='){
                        token.type = TYPE_LESSEQ;
                        return token;
                    }
                    // Checks if token is type <?php and if it is, checks its validity
                    else if (c == '?'){
                        for (int i = 0; i < 3; i++){
                            DS_append(dynamicString, c = getc(source));
                        }
                        if (!strcmp(DS_string(dynamicString), "php") && isspace((c = getc(source)))){
                            ungetc(c, source);
                            token.type = TYPE_BEGIN;
                            DS_dispose(dynamicString);
                            return token;
                        }
                        fprintf(stderr, "Incorrect prolog on line %d!\nProlog should be: \"<?php\" with whitespace behind it!\n", token.rowNumber);
                        token.type = TYPE_LEXERR;
                        return token;
                    }
                    ungetc(c, source);
                    // If token is type <, return it
                    token.type = TYPE_LESS;
                    return token;
                }
                // >, >=
                else if (c == '>'){
                    // Checks if token is type >=
                    c = getc(source);
                    if (c == '='){
                        token.type = TYPE_GREATEREQ;
                        return token;
                    }
                    ungetc(c, source);
                    // If token is type >, return it
                    token.type = TYPE_GREATER;
                    return token;
                }
                // STRING
                else if (c == '"'){
                    state = STATE_STRING;
                    break;
                }
                // VARIABLE
                else if (c == '$'){
                    state = STATE_VAR;
                    ungetc(c, source);
                    break;
                }
                // FUNCTION
                else if (isalpha(c) || c == '_'){
                    state = STATE_ID;
                    ungetc(c, source);
                    break;
                }
                // Unknown char
                else{
                    fprintf(stderr, "%c is unknown character on line %d!\n", c, token.rowNumber);
                    token.type = TYPE_LEXERR;
                    return token;
                }
                break;

            // State for returning whole string
            case STATE_STRING: 
                // Checks if string is empty: ""
                if (c == '"'){
                    token.type = TYPE_STRING;
                    token.attribute.dString = DS_init();
                    return token;
                }
                // If string is not empty:
                while (1){
                    // Checks if c is \, if it is, store it and store next char in case, that c = "
                    if (c == '\\'){
                        DS_append(dynamicString, c);
                        c = getc(source);
                    }
                    DS_append(dynamicString, c);
                    c = getc(source);
                    // Then repeat it until end of string "
                    if (c == '"'){
                        token.type = TYPE_STRING;
                        token.attribute.dString = dynamicString;
                        return token;
                    }
                    else if (c == EOF){
                        fprintf(stderr, "Unclosed string on line %d!\nExpected: \"\n", token.rowNumber);
                        token.type = TYPE_LEXERR;
                        return token;
                    }
                }
                break;

            // State for returning names of variables
            case STATE_VAR:
                DS_append(dynamicString, c);
                c = getc(source);
                if (!isalpha(c) && c != '_'){
                    fprintf(stderr, "Variables in IFJ22 should begin with alphabetic character or underscore on line %d!\nFor examle: \"$a...\" or \"$A...\" or \"$_...\"\n", token.rowNumber);
                    token.type = TYPE_LEXERR;
                    return token;
                }
                while (1){
                    DS_append(dynamicString, c);
                    c = getc(source);
                    // If character is not in set of characters for variable, stop and return token without the last read char, last char return back for next reading 
                    if (!isalnum(c) && c != '_'){
                        token.type = TYPE_ID;
                        token.attribute.dString = dynamicString;
                        ungetc(c, source);
                        return token;
                    }
                }
                break;

            // State for returning ID, KEYWORD or declare(strict_types=1)
            case STATE_ID:
                while (1){
                    if (isalnum(c) || c == '_' || c == '('){
                        if (c == '('){
                            // Check if id == "declare" and if it is clear dynamic string for *
                            if (!strcmp(DS_string(dynamicString), "declare")){
                                DS_deleteAll(dynamicString);
                                c = getc(source);
                                // * loading chars in brackets and then check if it is "strict_types=1"
                                while (1){
                                    DS_append(dynamicString, c);
                                    c = getc(source);
                                    if (c == ')'){
                                        if (!strcmp(DS_string(dynamicString), "strict_types=1")){
                                            token.type = TYPE_DECLARE_ST;
                                            DS_dispose(dynamicString);
                                            return token;
                                        }
                                        fprintf(stderr, "Unknown define of strict types on line %d!\nExpected: \"declare(strict_types=1)\"\n", token.rowNumber);
                                        token.type = TYPE_LEXERR;
                                        return token;
                                    }
                                    if (c == EOF){
                                        fprintf(stderr, "Unclosed bracked on line %d!\nExpected: \")\"\n", token.rowNumber);
                                        token.type = TYPE_LEXERR;
                                        return token;
                                    }
                                }
                            }
                            // If id != declare stop loading chars and *
                            break;
                        }
                        DS_append(dynamicString, c);
                        c = getc(source);
                        continue;
                    }
                    break;
                }
                // *return FUNID if its not keyword, if it is return keyword in function "isKeyword" and return symbol ( back to queue
                if (isKeyword(dynamicString, &token)){
                        ungetc(c, source);
                        DS_dispose(dynamicString);
                        return token;
                }
                ungetc(c, source);
                token.type = TYPE_FUNID;
                token.attribute.dString = dynamicString;
                return token;

            // State for returning integer or jump to float
            case STATE_INTEGER:
                while(1){
                    if (isdigit(c) || c == 'e' || c == '.' || c == 'E'){
                        if (c == 'e' || c == '.' || c == 'E'){
                            state = STATE_FLOAT;
                            ungetc(c, source);
                            break;
                        }
                        DS_append(dynamicString, c);
                        c = getc(source);
                        continue;
                    }
                    token.type = TYPE_INT;
                    token.attribute.intV = atoi(DS_string(dynamicString));
                    DS_dispose(dynamicString);
                    ungetc(c, source);
                    return token;
                }
                break;

            // State for returning float
            case STATE_FLOAT:
                while (1){
                    if (isdigit(c) || c == 'e' || c == '.' || c == '-' || c == 'E' || c == '+'){
                        if (((c == 'e' || c == 'E') && wasE) || (c == '.' && wasDot)){
                            fprintf(stderr, "Wrong float number on line %d!\nSymbol . or character e can be used only once in float!\nExpected: 1.2 or 1.2e10 or 1.2e-10\n", token.rowNumber);
                            token.type = TYPE_LEXERR;
                            return token;
                        }
                        if (c == 'e' || c == 'E'){
                            wasE = 1;
                        }
                        if (c == '.'){
                            wasDot = 1;
                        }
                        DS_append(dynamicString, c);
                        if (c == 'e' || c == 'E'){
                            if ((c = getc(source)) == '-' || c == '+'){
                                int tmp = c;
                                if (!isdigit(c = getc(source))){
                                    fprintf(stderr, "Wrong float number on line %d!\nExpected number after + or -: \"2e-1 or 2e+3\"\n", token.rowNumber);
                                    token.type = TYPE_LEXERR;
                                    return token;
                                }
                                ungetc(c, source);
                                DS_append(dynamicString, tmp);
                            }
                            else{
                                ungetc(c, source);
                            }
                        }
                        c = getc(source);
                        if (c == '-' || c == '+'){
                            ungetc(c, source);
                            break;
                        }
                        continue;
                    }
                    break;
                }
                token.type = TYPE_FLOAT;
                token.attribute.doubleV = atof(DS_string(dynamicString));
                DS_dispose(dynamicString);
                ungetc(c, source);
                return token;
        }
    }
}