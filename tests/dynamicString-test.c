#include "../dynamicString.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    DynamicString *string = DS_init();

    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    char newString[] = "0123456789";
    DS_appendString(string, newString);
    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    DS_appendString(string, newString);
    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    DS_deleteAll(string);
    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    DS_appendString(string, newString);
    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    DS_dispose(string);
    printf("l:%d c:%d |%s|\n", string->length, string->numOfChars, string->string);
    return 0;
}