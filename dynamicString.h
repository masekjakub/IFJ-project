/**
 * @file dynamicString.h
 * @author Vojtěch Kuchař
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
void DS_deleteChar(DynamicString *dynamicString);
void DS_deleteAll(DynamicString *dynamicString);
void DS_dispose(DynamicString *dynamicString);

#endif