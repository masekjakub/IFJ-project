#include <stdio.h>
#include "scanner.h"
#include "parser.h"

//  Pro ucely testovani
int main(int argc, char** argv){
    FILE *f;
    Token token;
    ErrorType error = 0;

   /* if ((f = fopen(stdin, "r")) == NULL){
        fprintf(stderr, "Soubor neslo otevrit\n");
        return ERR_OTHER;
    }*/

    f = stdin;
    setSourceFile(f);
    error = parser();
    
    // Filip tester - vymazat
    /*for (int i = 0; i < 51; i++){
        token = getToken();
        if (token.type == TYPE_STRING || token.type == TYPE_ID || token.type == TYPE_FUNID){
            printf("%d --- %s\n",token.type  ,token.attribute.dString->string);
            DS_dispose(token.attribute.dString);
        }
        else if (token.type == TYPE_INT){
            printf("%d --- %d\n",token.type ,token.attribute.intV);
        }
        else if (token.type == TYPE_FLOAT){
            printf("%d --- %f\n",token.type ,token.attribute.doubleV);
        }
        else{
            printf("%d type\n", token.type);
        }
    }*/
    fclose(f);
    exit(error);
}