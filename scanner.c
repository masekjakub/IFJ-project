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
 * @brief Get the Token object
 * 
 * @return Token 
 */
Token getToken(){
    int c;
    State state = STATE_START;
    DynamicString *dynamicString = DS_init();
    while (1){
        Token token;
        c = getc(source);
        switch (state){
            // Starting state of finite automat
            case STATE_START:
                if (isspace(c)){
                    state = STATE_START;
                    break;
                }
                else if (c == '/'){
                    c = getc(source);
                    if (c == '/'){
                        while (c != '\n'){ c = getc(source); }
                        token.type = TYPE_COMM;
                        return token;
                    }
                    else if (c == '*'){ 
                        c = getc(source);
                        while (1){
                            if (c == '*'){
                                if ((c = getc(source)) == '/'){
                                    token.type = TYPE_COMM;
                                    return token;
                                }
                                ungetc(c, source);
                            }
                            else if (c == EOF){
                                fprintf(stderr, "Expected end of comment: \"*/\"!\n");
                                exit(ERR_LEX);
                            }
                            c = getc(source);
                        }
                    }
                    ungetc(c, source);
                    token.type = TYPE_DIV;
                    return token;
                }
                else if (c == EOF){
                    token.type = TYPE_EOF;
                    return token;
                }
                else if (c == '%'){
                    token.type = TYPE_MOD;
                    return token;
                }
                else if (c == '*'){
                    token.type = TYPE_MUL;
                    return token;
                }
                else if (c == '+'){
                    token.type = TYPE_ADD;
                    return token;
                }
                else if (c == '.'){
                    token.type = TYPE_CONCAT;
                    return token;
                }
                else if (c == ';'){
                    token.type = TYPE_SEMICOLON;
                    return token;
                }
                else if (c == '('){
                    token.type = TYPE_LBRACKET;
                    return token;
                }
                else if (c == ')'){
                    token.type = TYPE_RBRACKET;
                    return token;
                }
                else if (c == '{'){
                    token.type = TYPE_LBRACES;
                    return token;
                }
                else if (c == '}'){
                    token.type = TYPE_RBRACES;
                    return token;
                }
                //TODO typy stringu ?int ?string ?float
                else if (c == '?'){
                    if ((c = getc(source)) == '>'){
                        token.type = TYPE_END;
                        return token;
                    }
                    fprintf(stderr, "Wrong epilog!\nExpected epilog: \"?>\"\n");
                    exit(ERR_LEX);
                }
                else if (c == '='){
                    // Checks if token is type === or invalid type ==
                    c = getc(source);
                    if (c == '='){
                        if ((c = getc(source)) == '='){
                            token.type = TYPE_EQTYPES;
                            return token;
                        }
                        fprintf(stderr, "Non-existing operator \"==\"!\nThere are only operands: \"=\" or \"===\".\n");
                        exit(ERR_LEX);
                    }
                    ungetc(c, source);
                    // If token is type =, return it
                    token.type = TYPE_ASSIGN;
                    return token;
                }
                else if (c == '!'){
                    // Checks if token is type !== or invalid type !=
                    c = getc(source);
                    if (c == '='){
                        if ((c = getc(source)) == '='){
                            token.type = TYPE_NOTEQTYPES;
                            return token;
                        }
                        fprintf(stderr, "Non-existing operator \"!=\"!\nThere are only operands: \"!\" or \"!==\".\n");
                        exit(ERR_LEX);
                    }
                    ungetc(c, source);
                    // If token is type !, return it
                    token.type = TYPE_NEG;
                    return token;
                }
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
                            token.type = TYPE_BEGIN;
                            DS_dispose(dynamicString);
                            return token;
                        }
                        fprintf(stderr, "Incorrect prolog!\nProlog should be: \"<?php\" with whitespace behind it!\n");
                        exit(ERR_LEX);
                    }
                    ungetc(c, source);
                    // If token is type <, return it
                    token.type = TYPE_LESS;
                    return token;
                }
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
                else if (c == '"'){
                    state = STATE_STRING;
                    break;
                }
                else if (c == '$'){
                    state = STATE_VAR;
                    break;
                }
                else{//TODO pobavit se o tom s martenem
                    fprintf(stderr, "%c is unknown character!\n", c);
                    exit(ERR_LEX);
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
                        c = getc(source);
                        return token;
                    }
                    else if (c == EOF){
                        fprintf(stderr, "Unclosed string!\nExpected: \"\n");
                        exit(ERR_LEX);
                    }
                }
                break;
            // State for returning names of variables
            case STATE_VAR:
                if (!isalpha(c) && c != '_'){
                    fprintf(stderr, "Variables in IFJ22 should begin with alphabetic character or underscore!\nFor examle: \"$a...\" or \"$A...\" or \"$_...\"\n");
                    exit(ERR_LEX);
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
        }
    }
}

//  Pro ucely testovani
int main(int argc, char** argv){
    FILE *f;
    Token token;
    
    if (argc == 1){
        fprintf(stderr, "Chybi vstupni soubor\n");
        return ERR_OTHER;
    }
    if ((f = fopen(argv[1], "r")) == NULL){
        fprintf(stderr, "Soubor neslo otevrit\n");
        return ERR_OTHER;
    }
    printf("Ahoj, svete\n");

    setSourceFile(f);
    for (int i = 0; i < 20; i++){
        token = getToken();
        printf("%d\n", token.type);
        if (token.type == TYPE_STRING || token.type == TYPE_ID){
            printf("%s\n", token.attribute.dString->string);
            DS_dispose(token.attribute.dString);
        }
    }

    fclose(f);
}