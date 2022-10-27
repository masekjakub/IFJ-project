#include "../dynamicString.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    DynamicString* string = DS_init();
    char char1 = 'c';
    DS_append(string, char1);
    //printf("string: %s", string);
    return 0;
}