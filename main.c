/**
 * @file main.c
 * @authors Jakub Mašek
 * @brief Main file for IFJ22 compiler
 * @version 1.0
 * @date 2022-12-7
 */

#include <stdio.h>
#include "scanner.h"
#include "parser.h"

/**
 * @brief Main function of the compiler
 * 
 * @return int error code
 */
int main(){
    ErrorType error = 0;

    // Set source file for scanner
    setSourceFile(stdin);

    // Call parser
    error = parser();
    
    fclose(stdin);
    exit(error);
}