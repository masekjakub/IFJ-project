/**
 * @file dynamicString.c
 * @author Vojtěch Kuchař
 * @brief Functions operating dynamic string
 * @version 0.1
 * @date 2022-10-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "dynamicString.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes a dynamic string with initial length of 256 chars.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 * @return DynamicString
 */
DynamicString *DS_init()
{
    DynamicString *dynamicString = (DynamicString *)malloc(sizeof(DynamicString));
    dynamicString->string = calloc(256, sizeof(char));
    dynamicString->numOfChars = 0;
    dynamicString->string[0] = '\0';
    return dynamicString;
}

/**
 * @brief Function appends dynamic string by one character.
 * If string is full, function doubles it´s length, then appends string by one character.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 * @param character Character appending the string.
 * @return DynamicString
 */
DynamicString *DS_append(DynamicString *dynamicString, char character)
{
    /*if (dynamicString->length == dynamicString->numOfChars)
    {
        if (realloc(dynamicString->string, 2 * dynamicString->length * sizeof(char)) == NULL)
        {
            exit(ERR_INTERN);
        }
        else
        {
            dynamicString->length *= 2;
            dynamicString->string[dynamicString->numOfChars] = character;
            dynamicString->string[dynamicString->numOfChars + 1] = '\0';
            dynamicString->numOfChars++;
        }
    }
    else
    {
        dynamicString->string[dynamicString->numOfChars] = character;
        dynamicString->numOfChars++;
    }*/
    return dynamicString;
}

/**
// * @brief Function appends dynamic string by inserted string.
// * If string is full, function doubles it´s length and adds length to it.
// *
// * @param dynamicString Dynamic structure of DynamicString type.
// * @param string String appending the Dynamic string.
// * @return DynamicString
// */
// DynamicString DS_insertString(DynamicString dynamicString, char *string, int length)
//{
//     if (dynamicString.length <= dynamicString.numOfChars + length)
//     {
//         if (realloc(dynamicString.string, length + 2 * dynamicString.length * sizeof(char)) == NULL)
//         {
//             exit(ERR_INTERN);
//         }
//         else
//         {
//             dynamicString.length = 2 * dynamicString.length + length;
//             for (int i = 0; i < length - 1; i++)
//             {
//                 dynamicString.string[dynamicString.numOfChars + i] = string[i];
//             }
//             dynamicString.numOfChars = dynamicString.numOfChars + length;
//             dynamicString.string[dynamicString.numOfChars + length] = '\0';
//         }
//     }
//     return dynamicString;
// }
//
///**
// * @brief Function erases last character of dynamic string.
// *
// * @param dynamicString Dynamic structure of DynamicString type.
// */
// void DS_deleteChar(DynamicString dynamicString)
//{
//    if (dynamicString.numOfChars != 0)
//    {
//        dynamicString.string[dynamicString.numOfChars - 1] = '\0';
//        dynamicString.numOfChars--;
//    }
//}
//
//// void DS_deleteAll();
//
// void DS_dispose(DynamicString *dynamicString)
//{
//    // TODO zbytek lol
//    free(dynamicString);
//}