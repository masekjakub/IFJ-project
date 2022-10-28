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

//State state = STATE_PROLOG;

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

Token getToken(){
    int c;
    int i = 0;
    State state = STATE_START;
    char array[20];
    char phpStart[4];
    while (1){
        Token token;
        c = getc(source);
        switch (state){
            // state which controls right begining of file
            //case STATE_PROLOG:
            //    if (isspace(c) || c == '<'){
            //        if (c == '<'){
            //            ungetc(c, source);
            //            for (int i = 0; i < 5; i++){
            //                c = getc(source);
            //                phpStart[i] = c;  // Nahradi se za dynamic string
            //            }
            //            if (strcmp(phpStart, "<?php")){
            //                if (isspace(c = getc(source))){
            //                    token.type = TYPE_BEGIN;
            //                    state = STATE_START;
            //                    return token;
            //                }
            //                fprintf(stderr, "After prolog \"<?php\" must be whitespace!");
            //                exit(ERR_LEX);
            //            }
            //            fprintf(stderr, "Prolog at the start must be \"<?php\"!\n");
            //            exit(ERR_LEX);
            //        }
            //    }
            //    fprintf(stderr, "Before prolog \"<?php\" can be only whitespaces!\n");
            //    exit(ERR_LEX);
           
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
                            if (c == '*' && (c = getc(source)) == '/'){
                                token.type = TYPE_COMM;
                                return token;
                            }
                            if (c == EOF){
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
                else if (c == '='){
                    c = getc(source);
                    if (c == '=' && (c = getc(source)) == '='){
                        token.type = TYPE_EQTYPES;
                        return token;
                    }
                    ungetc(c, source);
                    token.type = TYPE_ASSIGN;
                    return token;
                }
                else{
                    token.type = 67;
                    return token;
                    break;
                }
                break;
            //case 1: 
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
    }

    fclose(f);
}