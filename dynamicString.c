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
 * @brief Initializes a dynamic string with initial length of 8 chars.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 * @return DynamicString
 */
DynamicString *DS_init()
{
    DynamicString *dynamicString = (DynamicString *)malloc(sizeof(DynamicString));
    dynamicString->string = calloc(8, sizeof(char));
    dynamicString->length = 8;
    dynamicString->numOfChars = 0;
    dynamicString->string[0] = '\0';
    return dynamicString;
}

/**
 * @brief Function appends dynamic string by one character.
 * If string is full, function doubles it´s length, then appends string by the character.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 * @param character Character appending the string.
 * @return DynamicString
 */
DynamicString *DS_append(DynamicString *dynamicString, char character)
{
    if (dynamicString->length == dynamicString->numOfChars)
    {
        dynamicString->string = realloc(dynamicString->string, (2 * dynamicString->length + 1) * sizeof(char));
        if (dynamicString->string == NULL)
        {
            exit(ERR_INTERN);
        }
        else
        {
            dynamicString->length *= 2;
        }
    }
    dynamicString->string[dynamicString->numOfChars] = character;
    dynamicString->string[dynamicString->numOfChars + 1] = '\0';
    dynamicString->numOfChars++;
    // printf("l:%d c:%d |%s|\n", dynamicString->length, dynamicString->numOfChars, dynamicString->string);
    return dynamicString;
}

/**
 * @brief Function appends dynamic string by inserted string.
 * If string is full, function doubles it´s length and appends DS by string.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 * @param string String appending the dynamic string.
 * @return DynamicString
 */
DynamicString *DS_appendString(DynamicString *dynamicString, char *string)
{
    int length = (int)strlen(string);
    for (int i = 0; i < length; i++)
    {
        DS_append(dynamicString, string[i]);
    }
    return dynamicString;
}

/**
 * @brief Function erases last character of dynamic string.
 * If string is at least half empty, function cuts it`s capacity by half.
 *
 * @param dynamicString Dynamic structure of DynamicString type.
 */
void DS_deleteChar(DynamicString *dynamicString)
{
    if (dynamicString->length >= 2 * dynamicString->numOfChars)
    {
        if (realloc(dynamicString->string, dynamicString->length * sizeof(char) / 2) == NULL)
        {
            exit(ERR_INTERN);
        }
        else
        {
            dynamicString->length /= 2;
        }
    }
    if (dynamicString->numOfChars != 0)
    {
        dynamicString->string[dynamicString->numOfChars - 1] = '\0';
        dynamicString->numOfChars--;
    }
}
/**
 * @brief Deteles all characters in dynamic string except the first one.
 * The first character is replaced by '\0'.
 *
 * @param dynamicString
 */
void DS_deleteAll(DynamicString *dynamicString)
{
    int length = (int)strlen(dynamicString->string);
    for (int i = 0; i < length; i++)
    {
        DS_deleteChar(dynamicString);
    }
}

/**
 * @brief Frees all alocated memory, by dynamic string.
 *
 * @param dynamicString
 */
void DS_dispose(DynamicString *dynamicString)
{
    free(dynamicString->string);
    free(dynamicString);
}