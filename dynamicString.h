#ifndef _IFJ_DS
#define _IFJ_DS

typedef struct
{
    char *string;
    int numOfChars;
    int length;
} DynamicString;

DynamicString *DS_init();
DynamicString *DS_append(DynamicString *dynamicString, char character);
DynamicString *DS_appendString(DynamicString *dynamicString, char *string);
void DS_deleteChar(DynamicString *dynamicString);
void DS_deleteAll(DynamicString *dynamicString);
void DS_dispose(DynamicString *dynamicString);

#endif