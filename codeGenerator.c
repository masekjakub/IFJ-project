#include "codeGenerator.h"

#define formatString2string(DEST, FORMAT, FORMAT_ARGS...)   DEST = malloc((snprintf(NULL,0,FORMAT, FORMAT_ARGS)+1)*sizeof(char));   \
                                                            if(DEST == NULL) exit(ERR_INTERN);                                      \
                                                            sprintf(DEST, FORMAT, FORMAT_ARGS);

int CODEcheckAndConvert2SameType(DynamicString *dString, char *varName1, char *varName2){
    return 0;
}

/**
 * @brief generate start of main
 * 
 * @param dString 
 */
void CODEgenerateMain(DynamicString *dString){
    DS_appendString(dString, "######MAIN######\n");
    DS_appendString(dString, "LABEL _main\n");
    DS_appendString(dString, "CREATEFRAME\n");
    DS_appendString(dString, "PUSHFRAME\n");
}

/**
 * @brief Checks the type of given variable and
 * converts it ot given type indicated by char type.
 * @param varName Name of varible to convert
 * @param dString Dynamic string to append the code to
 * @param type Char representing the type to convert to:
 * 'i'=int ,'b'=bool, 'f'=float, 's'=string, 'n'=nil
 * @return int 
 */
int CODEconvert2Type(DynamicString *dString, char *varName, char type){
    static int varNameNum = 0;
    varNameNum++;
    
    char *codeFormat = "\
CREATEFRAME\n\
DEFVAR TF@%%convType%d\n\
TYPE TF@%%convType%d %s\n\
"; //, varNameNum, varNameNum, varName

    char *code = NULL;
    formatString2string(code, codeFormat, varNameNum, varNameNum, varName);
    DS_appendString(dString, code);
    free(code);

    switch (type){
    case 'i':
        codeFormat="\
JUMPIFEQ _convFloat2Int%d TF@%%convType%d string@float\n\
JUMPIFEQ _convNil2Int%d TF@%%convType%d string@nil\n\
JUMP _noConv%d\n\
\n\
LABEL _convFloat2Int%d\n\
FLOAT2INT %s %s\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2Int%d\n\
MOVE %s int@0\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName

        code = NULL;
        formatString2string(code, codeFormat, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'b':
        codeFormat="\
JUMPIFEQ _convInt2Bool%d TF@%%convType%d string@int\n\
JUMPIFEQ _convFloat2Bool%d TF@%%convType%d string@float\n\
JUMPIFEQ _convString2Bool%d TF@%%convType%d string@string\n\
JUMPIFEQ _convNil2Bool%d TF@%%convType%d string@nil\n\
JUMP _noConv%d\n\
\n\
LABEL _convInt2Bool%d\n\
JUMPIFNEQ _convInt2Bool%d_true %s int@0\n\
MOVE %s bool@false\n\
JUMP _noConv%d\n\
LABEL _convInt2Bool%d_true\n\
MOVE %s bool@true\n\
JUMP _noConv%d\n\
\n\
LABEL _convFloat2Bool%d\n\
JUMPIFNEQ _convFloat2Bool%d_true %s float@0x0p+0\n\
MOVE %s bool@false\n\
JUMP _noConv%d\n\
LABEL _convFloat2Bool%d_true\n\
MOVE %s bool@true\n\
JUMP _noConv%d\n\
\n\
LABEL _convString2Bool%d\n\
JUMPIFNEQ _convString2Bool%d_true %s string@\n\
MOVE %s bool@false\n\
JUMP _noConv%d\n\
LABEL _convString2Bool%d_true\n\
MOVE %s bool@true\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2Bool%d\n\
MOVE %s bool@false\n\
"; /*, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum
,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum
,varName,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varName
*/
        code = NULL;
        formatString2string(code, codeFormat, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum
        ,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum
        ,varName,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'f':
        codeFormat="\
JUMPIFEQ _convInt2Float%d TF@%%convType%d string@int\n\
JUMPIFEQ _convNil2Float%d TF@%%convType%d string@nil\n\
JUMP _noConv%d\n\
\n\
LABEL _convInt2Float%d\n\
INT2FLOAT %s %s\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2Float%d\n\
MOVE %s float@0x0p+0\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName
        code = NULL;
        formatString2string(code, codeFormat, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 's':
        codeFormat="\
JUMPIFEQ _convNil2String%d TF@%%convType%d string@nil\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2String%d\n\
MOVE %s string@\n\
JUMP _noConv%d\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varName,varNameNum
        code = NULL;
        formatString2string(code, codeFormat, varNameNum,varNameNum,varNameNum,varNameNum,varName,varNameNum);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'n':
        codeFormat="\
MOVE %s nil@nil\n\
"; //, varName
        code = NULL;
        formatString2string(code, codeFormat, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    default:
        exit(ERR_INTERN);
        break;
    }

    codeFormat="\
LABEL _noConv%d\n\n\
"; //, varNameNum

    code = NULL;
    formatString2string(code, codeFormat, varNameNum);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

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
    char *codeFormat = "\
DEFVAR LF@%%if_cond%d\n\
POPS LF@%%if_cond%d\n\
"; //, ifCount,ifCount

    char *code = NULL;
    formatString2string(code, codeFormat, ifCount,ifCount);
    DS_appendString(dString, code);
    free(code);

    //Conversion to bool
    codeFormat = "LF@%%if_cond%d";
    code = NULL;
    formatString2string(code, codeFormat, ifCount);
    CODEconvert2Type(dString,code,'b');

    codeFormat = "\
JUMPIFEQ _else%d LF@%%if_cond%d bool@false\n\
LABEL _if%d\n\
"; //, ifCount,ifCount,ifCount

    code = NULL;
    formatString2string(code, codeFormat, ifCount,ifCount,ifCount);
    DS_appendString(dString, code);
    free(code);

    /*char *codeFormat = "\
DEFVAR LF@%%if_cond%d\n\
POPS LF@%%if_cond%d\n\
JUMPIFEQ _else%d LF@%%if_cond%d bool@false\n\
LABEL _if%d\n\
"; //, ifCount,ifCount,ifCount,ifCount,ifCount

    char *code = NULL;
    formatString2string(code, codeFormat, ifCount,ifCount,ifCount,ifCount,ifCount);
    DS_appendString(dString, code);
    free(code);*/

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
JUMP _endif%d\n\
LABEL _else%d\n\
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
LABEL _endif%d\n\
";
    char *code = NULL;
    formatString2string(code, code_format, ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEpushValue(DynamicString *dString, Token token){
    char *code = NULL;
    char *code_format;

    switch (token.type)
    {
    case TYPE_FLOAT:
        
        code_format = "\
PUSHS float@%a\n\
";
        formatString2string(code, code_format,token.attribute.doubleV);
        DS_appendString(dString, code);
        free(code);
        break;
    
    default:
        break;
    }
    return 0;
}

int CODEgenerateFuncDef(DynamicString *dString, char *functionName){
    char *code = NULL;
    char *codeFormat = "\
LABEL _%s\n\
CREATEFRAME\n\
PUSHFRAME\n\
";
    formatString2string(code, codeFormat, functionName);
    DS_appendString(dString, code);
    free(code);
    return 0;
}

/**
 * @brief generates func call
 * 
 * @param dString string to save in
 * @param token functionID token
 * @param argCount count of argument
 * @return int 
 */
int CODEgenerateFuncCall(DynamicString *dString, Token token, int argCount){
    char *code = NULL;
    char *code_format = "CALL _%s\n";
    formatString2string(code, code_format,DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
    return 0;
}

/**
 * @brief defines new var
 * 
 * @param dString string to save in
 * @param token variable token
 * @return int 
 */
int CODEdefVar(DynamicString *dString, Token token){
    char *code = NULL;
    char *code_format = "DEFVAR LF@%s\n";
    formatString2string(code, code_format,DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
    return 0;
}

int CODEassign(DynamicString *dString, Token token){
char *code = NULL;
char *code_format = "\
POPS LF@%s\n";
formatString2string(code, code_format,DS_string(token.attribute.dString));
DS_appendString(dString, code);
free(code);
return 0;
}

// read, write + zadani str. 10, udelat: ulozit do symtable, generovat kod
int CODEgenerateBuiltInFunc(DynamicString *dString)
{   
char *code = "\
.IFJcode22\n\
JUMP _main\n\n";
DS_appendString(dString, code);

//write
code = "\
######WRITE######\n\
LABEL _write\n\
CREATEFRAME\n\
DEFVAR TF@tmpwrite\n\
POPS TF@tmpwrite\n\
WRITE TF@tmpwrite\n\
CREATEFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

//readi
code = "\
######READI######\n\
LABEL _readi\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input int\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readiOk TF@input$type string@int\n\
PUSHS int@0\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readiOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

//readf
code = "\
######READF######\n\
LABEL _readf\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input float\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readfOk TF@input$type string@float\n\
PUSHS float@0x0p+0\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readfOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

//readf
code = "\
######READS######\n\
LABEL _reads\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input string\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readsOk TF@input$type string@string\n\
PUSHS string@\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readsOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

code = "\
LABEL _float2int\n\
CREATEFRAME\n\
DEFVAR LF@retval\n\
RETURN\n\
\n";
DS_appendString(dString, code);

code = "\
LABEL float2int$righttype\n\
POPFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);
    return 0;
}