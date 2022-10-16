#include "getTokenSim.h" //simulation DELETE ME
//#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Token *tokenArr = initTokens();

    Token token = getTokenSim();
    
    while(token.atribute.keyword == KEYWORD_EOF){
        serveSymTable(token);
        if (token.type == TYPE_DOUBLE || token.type == TYPE_INT){
            
        }
    }

    return 0;
}

serveSymTable(){

}