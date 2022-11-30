#include "codeGenerator.h"

#define formatString2string(DEST, FORMAT, FORMAT_ARGS...)   int formatedCodeLen = snprintf(NULL,0,FORMAT, FORMAT_ARGS)+1;   \
                                                            DEST = malloc(formatedCodeLen*sizeof(char));                    \
                                                            if(DEST == NULL) exit(ERR_INTERN);                              \
                                                            sprintf(DEST, FORMAT, FORMAT_ARGS);

/**
 * @brief Generates code for if statement header
 * Expects the condition value on stack
 * @param dString Dynamic string to append the code to
 * @param ifCount Number used for unique label
 * @return int 
 */
//udelat: konverze výsledku výrazu
int CODEifStart(DynamicString *dString, int ifCount)
{   
    char *code_format = "\
DEFVAR LF@%%if_cond%d\n\
POPS LF@%%if_cond%d\n\
JUMPIFEQ @_else%d LF@%%if_cond%d bool@false\n\
LABEL @_if%d\n\
";
    char *code = NULL;
    formatString2string(code, code_format, ifCount,ifCount,ifCount,ifCount,ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

/**
 * @brief Generates code for if statement else header
 * Expects the condition value on stack
 * @param dString Dynamic string to append the code to
 * @param ifCount Number used for unique label
 * @return int 
 */
int CODEelse(DynamicString *dString, int ifCount)
{   
    char *code_format = "\
JUMP @_endif%d\n\
LABEL @_else%d\n\
";
    char *code = NULL;
    formatString2string(code, code_format, ifCount,ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

/**
 * @brief Generates code for if statement end
 * Expects the condition value on stack
 * @param dString Dynamic string to append the code to
 * @param ifCount Number used for unique label
 * @return int 
 */
int CODEendIf(DynamicString *dString, int ifCount)
{   
    char *code_format = "\
LABEL @_endif%d\n\
";
    char *code = NULL;
    formatString2string(code, code_format, ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEpushValue(DynamicString *dString, Token token){
    char *code ="\
    CREATEFRAME\n";
    DS_appendString(dString, code);

    switch (token.type)
    {
    case TYPE_FLOAT:
        DS_appendString(dString, "PUSHS ");
        sprintf(code, "%d", token.attribute.intV);
        DS_appendString(dString, code);
        break;
    
    default:
        break;
    }
    return 0;
}

// read, write + zadani str. 10, udelat: ulozit do symtable, generovat kod
int generateBuiltInFunc(DynamicString *dString)
{    char *code = "\
.IFJcode22\n\
JUMP @_main\n\
\n\
LABEL @_write\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR TF@tmpwrite\n\
POPS TF@tmpwrite\n\
WRITE TF@tmpwrite\n\
RETURN\n\
\n\
LABEL @_readi\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@input\n\
DEFVAR LF@input$type\n\
READ LF@input int\n\
TYPE LF@input$type LF@input\n\
JUMPIFEQ $righttype LF@input$type int@0\n\
\n\
MOVE LF@input nill@nil\n\
\n\
LABEL $readi$righttype\n\
POPFRAME\n\
RETURN\n\
\n\
\n\
\n\
LABEL @_readf\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@input\n\
DEFVAR LF@input$type\n\
READ LF@input float\n\
TYPE LF@input$type LF@input\n\
JUMPIFEQ $righttype LF@input$type float@0\n\
\n\
MOVE LF@input nill@nil\n\
\n\
LABEL $readi$righttype\n\
POPFRAME\n\
RETURN\n\
\n\
\n\
\n\
LABEL @_reads\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@input\n\
DEFVAR LF@input$type\n\
READ LF@input string\n\
TYPE LF@input$type LF@input\n\
JUMPIFEQ $righttype LF@input$type string@0\n\
\n\
MOVE LF@input nill@nil\n\
\n\
LABEL $readi$righttype\n\
POPFRAME\n\
RETURN\n\
\n\
\n\
\n\
LABEL @_float2int\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@retval\n\
\
\n\
LABEL $float2int$righttype\n\
POPFRAME\n\
RETURN\n\
";

    DS_appendString(dString, code);
    return 0;
}