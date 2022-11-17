#include "codeGenerator.h"

// read, write + zadani str. 10, udelat: ulozit do symtable, generovat kod
int generateBuiltInFunc(DynamicString *dString)
{

    // readi
    char *code = "                                  \
    LABEL readi\n                                   \
    CREATEFRAME\n                                   \
    PUSHFRAME\n                                     \
    DEFVAR LF@input\n                               \
    DEFVAR LF@input$type\n                          \
    READ LF@input int\n                             \
    TYPE LF@input$type LF@input\n                   \
    JUMPIFEQ $righttype LF@input$type int@0\n       \
                                                    \
    MOVE LF@input nill@nil\n                        \
                                                    \
    LABEL $readi$righttype\n                        \
    POPFRAME\n                                      \
    RETURN\n                                        \
                                                    \
                                                    \
                                                    \
    LABEL readf\n                                   \
    CREATEFRAME\n                                   \
    PUSHFRAME\n                                     \
    DEFVAR LF@input\n                               \
    DEFVAR LF@input$type\n                          \
    READ LF@input float\n                           \
    TYPE LF@input$type LF@input\n                   \
    JUMPIFEQ $righttype LF@input$type float@0\n     \
                                                    \
    MOVE LF@input nill@nil\n                        \
                                                    \
    LABEL $readi$righttype\n                        \
    POPFRAME\n                                      \
    RETURN\n                                        \
                                                    \
                                                    \
                                                    \
    LABEL reads\n                                   \
    CREATEFRAME\n                                   \
    PUSHFRAME\n                                     \
    DEFVAR LF@input\n                               \
    DEFVAR LF@input$type\n                          \
    READ LF@input string\n                          \
    TYPE LF@input$type LF@input\n                   \
    JUMPIFEQ $righttype LF@input$type string@0\n    \
                                                    \
    MOVE LF@input nill@nil\n                        \
                                                    \
    LABEL $readi$righttype\n                        \
    POPFRAME\n                                      \
    RETURN\n                                        \
    ";

    DS_appendString(dString, code);
    return 0;
}