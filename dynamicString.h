/**
 * @file dynamicString.h
 * @author Vojtěch Kuchař, Martin Zelenák(DS_insertString)
 * @brief Dynamic string header file
 * @version 1.0
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */

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
DynamicString *DS_insertString(DynamicString **dynamicStringPtr, char *string, int index);
void DS_deleteChar(DynamicString *dynamicString);
void DS_deleteAll(DynamicString *dynamicString);
void DS_dispose(DynamicString *dynamicString);
char *DS_string(DynamicString *);

#endif