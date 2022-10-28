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
                        while (c != '*' && (c = getc(source)) != '/'){
                            if (c == EOF){
                                fprintf(stderr, "Expected end of comment: \"*/\"!\n");
                                exit(ERR_LEX);
                            }
                        }
                        token.type = TYPE_COMM;
                        return token;
                    }
                    ungetc(c, source);
                    token.type = TYPE_DIV;
                    return token;
                }
                else{
                    printf("1\n");
                    return token;
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
    token = getToken();
    printf("%d\n", token.type);
    token = getToken();
    printf("%d\n", token.type);
    token = getToken();

    fclose(f);
}