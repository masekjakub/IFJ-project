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

Token getToken(){
    int c;
    int state = 0;
    int i = 0;
    char array[20];
    char phpStart[4];
    while (i<20){
        Token token;
        c = getc(source);
        switch (state){
            // state which controls right begining of file
            case 0:
                if (isspace(c) || c == '<'){
                    if (c == '<'){
                        ungetc(c, source);
                        for (int i = 0; i < 5; i++){
                            c = getc(source);
                            phpStart[i] = c;  // Nahradi se za dynamic string
                        }
                        if (strcmp(phpStart, "<?php")){
                            if (isspace(c = getc(source))){
                                token.type = TYPE_BEGIN;
                                return token;
                            }
                            fprintf(stderr, "After prolog \"<?php\" must be whitespace!");
                            exit(ERR_LEX);
                        }
                        fprintf(stderr, "Prolog at the start must be \"<?php\"!\n");
                        exit(ERR_LEX);
                    }
                }
                fprintf(stderr, "Before prolog \"<?php\" can be only whitespaces!\n");
                exit(ERR_LEX);

            // sdfsdfsafdfsf vojto uz to dodelej!! xddd
            case 1:
                if (isspace(c)){ //TODO nebo komentar
                    state = 1;
                    printf(" ");
                }
                else if(c == '$'){
                    state = 1;
                    //ungetc(c, source); asi byt nemusi protoze TYPE_ID
                }
                break;
            //case 1:
                
        }
        i++;
    }
}

//  Pro ucely testovani
int main(int argc, char** argv){
    FILE *f;
    
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
    getToken();

    fclose(f);
}