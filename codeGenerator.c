#include "codeGenerator.h"

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
";

    DS_appendString(dString, code);
    return 0;
}