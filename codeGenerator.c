#include "codeGenerator.h"

#define formatString2string(DEST, FORMAT, FORMAT_ARGS...)                       \
    DEST = malloc((snprintf(NULL, 0, FORMAT, FORMAT_ARGS) + 1) * sizeof(char)); \
    if (DEST == NULL)                                                           \
        exit(ERR_INTERN);                                                       \
    sprintf(DEST, FORMAT, FORMAT_ARGS);
#define isLower(CHAR) ('a' <= CHAR && CHAR <= 'z')

/**
 * @brief generate start of main
 *
 * @param dString
 */
void CODEmain(DynamicString *dString)
{
    DS_appendString(dString, "######MAIN######\n");
    DS_appendString(dString, "LABEL _main\n");
    DS_appendString(dString, "DEFVAR GF@void\n");
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
int CODEconvert2Type(DynamicString *dString, char *varName, char type)
{
    static int varNameNum = 0;
    varNameNum++;

    char *codeFormat = "\
#CODEconvert2Type\n\
CREATEFRAME\n\
DEFVAR TF@%%convType%d\n\
TYPE TF@%%convType%d %s\n\
"; //, varNameNum, varNameNum, varName

    char *code = NULL;
    formatString2string(code, codeFormat, varNameNum, varNameNum, varName);
    DS_appendString(dString, code);
    free(code);

    switch (type)
    {
    case 'i':
        codeFormat = "\
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
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'b':
        codeFormat = "\
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
JUMPIFEQ _convString2Bool%d_false %s string@\n\
JUMPIFEQ _convString2Bool%d_false %s string@0\n\
MOVE %s bool@true\n\
JUMP _noConv%d\n\
LABEL _convString2Bool%d_false\n\
MOVE %s bool@false\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2Bool%d\n\
MOVE %s bool@false\n\
"; /*, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum
,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum
,varName,varNameNum,varNameNum,varNameNum,varName,varNameNum,varName,varName,varNameNum,varNameNum,varName,varNameNum,varNameNum,varName
*/
        code = NULL;
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName, varNameNum, varNameNum, varNameNum, varName, varNameNum, varName, varName, varNameNum, varNameNum, varName, varNameNum, varNameNum, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'f':
        codeFormat = "\
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
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 's':
        codeFormat = "\
JUMPIFEQ _convNil2String%d TF@%%convType%d string@nil\n\
JUMP _noConv%d\n\
\n\
LABEL _convNil2String%d\n\
MOVE %s string@\n\
JUMP _noConv%d\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varName,varNameNum
        code = NULL;
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varName, varNameNum);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'n':
        codeFormat = "\
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

    codeFormat = "\
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
 * @param dStringPtr Ptr to ptr to dynamic string to append the code to
 * @param ifCount Number used for unique label
 * @param lastUnconditionedLine Index of last line in dString outside
 * any 'if' or 'while' code block.
 * -1 = not inside conditioned code block
 * @return int
 */
int CODEifStart(DynamicString **dStringPtr, int ifCount, int lastUnconditionedLine)
{
    DynamicString *dString = *dStringPtr;
    char *code = NULL;
    char *codeFormat = "\
#CODEifStart\n\
";
    DS_appendString(dString, codeFormat);

    // Definition of 'if' condition variable
    codeFormat = "%%if_cond%d"; //, ifCount
    formatString2string(code, codeFormat, ifCount);
    CODEdefVar(dStringPtr, code, lastUnconditionedLine);
    dString = *dStringPtr;
    free(code);

    codeFormat = "\
POPS LF@%%if_cond%d\n\
"; //, ifCount

    formatString2string(code, codeFormat, ifCount);
    DS_appendString(dString, code);
    free(code);

    // Conversion to bool
    codeFormat = "LF@%%if_cond%d";
    code = NULL;
    formatString2string(code, codeFormat, ifCount);
    CODEconvert2Type(dString, code, 'b');

    codeFormat = "\
JUMPIFEQ _else%d LF@%%if_cond%d bool@false\n\
"; //, ifCount,ifCount

    code = NULL;
    formatString2string(code, codeFormat, ifCount, ifCount);
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
    char *codeFormat = "\
#CODEelse\n\
JUMP _endif%d\n\
LABEL _else%d\n\
";
    char *code = NULL;
    formatString2string(code, codeFormat, ifCount, ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

/**
 * @brief Generates code for if statement end
 * Expects the condition value on stack
 * @param dStringPtr Ptr to ptr to dynamic string to append the code to
 * @param ifCount Number used for unique label
 * @return int
 */
int CODEifEnd(DynamicString *dString, int ifCount)
{
    char *codeFormat = "\
#CODEifEnd\n\
LABEL _endif%d\n\
";
    char *code = NULL;
    formatString2string(code, codeFormat, ifCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEwhileStart(DynamicString **dStringPtr, int whileCount, int lastUnconditionedLine)
{
    DynamicString *dString = *dStringPtr;
    char *code = NULL;
    char *codeFormat = "\
#CODEwhileStart\n\
";
    DS_appendString(dString, codeFormat);

    // Definition of 'while' condition variable
    codeFormat = "%%while_cond%d"; //, whileCount
    formatString2string(code, codeFormat, whileCount);
    CODEdefVar(dStringPtr, code, lastUnconditionedLine);
    dString = *dStringPtr;
    free(code);

    codeFormat = "\
LABEL _whileStart%d\n\
"; //, whileCount

    formatString2string(code, codeFormat, whileCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEwhileCond(DynamicString *dString, int whileCount)
{
    char *code = NULL;
    char *codeFormat = "\
#CODEwhileCond\n\
POPS LF@%%while_cond%d\n\
"; //, whileCount

    formatString2string(code, codeFormat, whileCount);
    DS_appendString(dString, code);
    free(code);

    // Conversion to bool
    codeFormat = "LF@%%while_cond%d";
    code = NULL;
    formatString2string(code, codeFormat, whileCount);
    CODEconvert2Type(dString, code, 'b');

    codeFormat = "\
JUMPIFEQ _whileEnd%d LF@%%while_cond%d bool@false\n\
"; //, whileCount,whileCount

    code = NULL;
    formatString2string(code, codeFormat, whileCount, whileCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEwhileEnd(DynamicString *dString, int whileCount)
{
    char *code = NULL;
    char *codeFormat = "\
JUMP _whileStart%d\n\
LABEL _whileEnd%d\n\
"; //, whileCount,whileCount

    formatString2string(code, codeFormat, whileCount, whileCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEpushValue(DynamicString *dString, Token token)
{
    char *code = NULL;
    char *codeFormat;

    switch (token.type)
    {
    case TYPE_FLOAT:

        codeFormat = "\
#CODEpushValue\n\
PUSHS float@%a\n\
";
        formatString2string(code, codeFormat, token.attribute.doubleV);
        DS_appendString(dString, code);
        free(code);
        break;

    default:
        break;
    }
    return 0;
}

//udelat: unused?
int CODEpopValue(DynamicString *dString, char *varName, bool isGlobalFrame)
{
    char *codeFormat;

    if (isGlobalFrame)
    {
        codeFormat = "\
#CODEpopValue\n\
POPS GF@%s\n\
"; //, varName
    }
    else
    {
        codeFormat = "\
#CODEpopValue\n\
POPS LF@%s\n\
";
    }

    char *code = NULL;
    formatString2string(code, codeFormat, varName);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEparam(DynamicString *dString, char *paramName)
{
    char *codeFormat = "\
#CODEparam\n\
DEFVAR LF@%s\n\
"; //, paramName

    char *code = NULL;
    formatString2string(code, codeFormat, paramName);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEpopParam(DynamicString *dString, char *paramName, char paramType)
{
    static int popValueCount = 0;
    popValueCount++;

    char *codeFormat;
    char *code = NULL;

    codeFormat = "\
#CODEpopParam\n\
POPS LF@%s\n\
CREATEFRAME\n\
DEFVAR TF@paramType\n\
TYPE TF@paramType LF@%s\n\
"; //, paramName,paramName

    formatString2string(code, codeFormat, paramName,paramName);
    DS_appendString(dString, code);
    free(code);

    if(!isLower(paramType)){
        codeFormat = "JUMPIFEQ _popParamRightType%d TF@paramType string@nil\n"; //, popValueCount
        formatString2string(code, codeFormat, popValueCount);
        DS_appendString(dString, code);
        free(code);

        paramType += 32; //Convert to lowercase
    }

    switch (paramType)
    {
    case 'i':
        codeFormat = "JUMPIFEQ _popParamRightType%d TF@paramType string@int\n"; //, popValueCount
        break;
    case 'f':
        codeFormat = "JUMPIFEQ _popParamRightType%d TF@paramType string@float\n"; //, popValueCount
        break;
    case 's':
        codeFormat = "JUMPIFEQ _popParamRightType%d TF@paramType string@string\n"; //, popValueCount
        break;
    default:
        fprintf(stderr,"Error calling CODEpopParam!\n");
        exit(ERR_INTERN);
        break;
    }
    formatString2string(code, codeFormat, popValueCount);
    DS_appendString(dString, code);
    free(code);

    codeFormat = "\
DPRINT string@Wrong\\032type\\032of\\032argument\\032%s\\032in\\032function\\032call!\n\
EXIT int@4\n\
LABEL _popParamRightType%d\n\
"; //, paramName,popValueCount

    formatString2string(code, codeFormat, paramName,popValueCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEfuncDef(DynamicString *dString, char *functionName)
{
    char *codeFormat = "\
######%s######\n\
#CODEfuncDef\n\
LABEL %s\n\
CREATEFRAME\n\
PUSHFRAME\n\
"; //, functionName,functionName

    char *code = NULL;
    formatString2string(code, codeFormat, functionName, functionName);
    DS_appendString(dString, code);
    free(code);
    return 0;
}

int CODEfuncReturn(DynamicString *dString, char returnType, int lineNum)
{
    static int returnNum = 0;
    returnNum++;

    char *code = "\
#CODEreturn\n\
CREATEFRAME\n\
DEFVAR TF@retVal\n\
POPS TF@retVal\n\
DEFVAR TF@retValType\n\
TYPE TF@retValType TF@retVal\n\
";
    DS_appendString(dString, code);

    // Convert returnType to lower case
    if (!isLower(returnType))
    {
        returnType += 32; // To lower case
    }

    // Type control
    char *codeFormat = NULL;
    switch (returnType)
    {
    case 'i':
        codeFormat = "\
JUMPIFEQ _rightReturnType%d TF@retValType string@int\n\
"; //, returnNum
        break;
    case 'f':
        codeFormat = "\
JUMPIFEQ _rightReturnType%d TF@retValType string@float\n\
"; //, returnNum
        break;
    case 's':
        codeFormat = "\
JUMPIFEQ _rightReturnType%d TF@retValType string@string\n\
"; //, returnNum
        break;
    default:
        fprintf(stderr, "Error in calling CODEfuncReturn()!\n");
        exit(ERR_INTERN);
        break;
    }

    code = NULL;
    formatString2string(code, codeFormat, returnNum);
    DS_appendString(dString, code);
    free(code);

    if (!isLower(returnType))
    { // If function can return null(nil)
        codeFormat = "\
JUMPIFEQ _rightReturnType%d TF@retValType string@nil\n\
";    //, returnNum
        code = NULL;
        formatString2string(code, codeFormat, returnNum);
        DS_appendString(dString, code);
        free(code);
    }

    codeFormat = "\
DPRINT string@Wrong\\032return\\032type\\032on\\032line\\032%d\\033\\010\n\
EXIT int@4\n\
LABEL _rightReturnType%d\n\
PUSHS TF@retVal\n\
POPFRAME\n\
CREATEFRAME\n\
RETURN\n\
"; //, lineNum,returnNum

    code = NULL;
    formatString2string(code, codeFormat, lineNum, returnNum);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int CODEfuncDefEnd(DynamicString *dString, char *funId, bool isVoid)
{
    char *code = NULL;
    if (isVoid)
    {
        code = "\
#CODEfuncDefEnd\n\
PUSHS nil@nil\n\
POPFRAME\n\
RETURN\n\n\
";
        DS_appendString(dString, code);
    }
    else
    {
        char *codeFormat = "\
#CODEfuncDefEnd\n\
DPRINT string@No\\032return\\032in\\032non-void\\032function\\032\"%s\"!\\010\n\
EXIT int@4\n\n\
"; //, funId
        formatString2string(code, codeFormat, funId);
        DS_appendString(dString, code);
        free(code);
    }

    return 0;
}

/**
 * @brief call write function with arguments in right order
 *
 * @param dString string to save in
 * @param token functionID token
 * @param argCount count of arguments
 * @return int
 */
int CODEcallWrite(DynamicString *dString, int argCount)
{
    if(argCount == 0){
        DS_appendString(dString, "PUSHS nil@nil\n");
        return 0;
    }

    char *code = "\
#CODEcallWrite\n\
CREATEFRAME\n\
DEFVAR TF@tmpwrite\n\
DEFVAR TF@writetype\n\
POPS TF@tmpwrite\n\
PUSHFRAME\n";
    DS_appendString(dString, code);
    argCount--;
    if (argCount > 0)
        CODEcallWrite(dString, argCount);
    code = "CALL write\n";
    DS_appendString(dString, code);
    if(argCount == 0){
        DS_appendString(dString, "PUSHS nil@nil\n");
        return 0;
    }
    return 0;
}

/**
 * @brief generates func call
 *
 * @param dString string to save in
 * @param token functionID token
 * @param argCount count of arguments
 * @return int
 */
int CODEfuncCall(DynamicString *dString, Token token, int argCount)
{
    if (!strcmp(DS_string(token.attribute.dString), "write"))
    {
        return CODEcallWrite(dString, argCount);
    }
    char *code = NULL;
    char *codeFormat = "\
#CODEfuncCall\n\
CALL %s\n\
";
    formatString2string(code, codeFormat, DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
    return 0;
}

/**
 * @brief defines new var on last unconditioned line (always performed)
 *
 * @param dString ptr of ptr to string to save in
 * @param varName Name of variable to be defined
 * @param lastUnconditionedLine
 * Index in dString, where definition of variable should be generated.
 * -1 = code will be appended
 * @return int
 */
int CODEdefVar(DynamicString **dStringPtr, char *varName, int lastUnconditionedLine)
{
    char *code = NULL;
    char *codeFormat = "\
#CODEdefVar\n\
DEFVAR LF@%s\n\
"; //, varName
    formatString2string(code, codeFormat, varName);
    if (lastUnconditionedLine == -1)
    {
        DS_appendString(*dStringPtr, code);
    }
    else
    {
        DS_insertString(dStringPtr, code, lastUnconditionedLine);
    }
    free(code);
    return 0;
}

int CODEassign(DynamicString *dString, Token token)
{
    char *code = NULL;
    char *codeFormat = "\
#CODEassign\n\
POPS LF@%s\n";
    formatString2string(code, codeFormat, DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
    return 0;
}

int CODEcheckInitVar(DynamicString *dString, char *varName, bool isGlobalFrame, int lineNum)
{
    static int labelCount = 0;
    labelCount++;

    char *code = NULL;
    char *codeFormat = "\
CREATEFRAME\n\
DEFVAR TF@%sType\n\
"; //, varName
    formatString2string(code, codeFormat, varName);
    DS_appendString(dString, code);
    free(code);

    if (isGlobalFrame)
    {
        codeFormat = "TYPE TF@%sType GF@%s\n"; //, varName,varName
    }
    else
    {
        codeFormat = "TYPE TF@%sType LF@%s\n"; //, varName,varName
    }
    formatString2string(code, codeFormat, varName, varName);
    DS_appendString(dString, code);
    free(code);

    codeFormat = "\
JUMPIFNEQ _varInitOk%d TF@%sType string@\n\
DPRINT string@Variable\\032%s\\032is\\032undefined\\032on\\032line\\032%d!\\010\n\
EXIT int@5\n\
LABEL _varInitOk%d\n\
CREATEFRAME\n\
"; //, labelCount,varName,varName,lineNum,labelCount
    formatString2string(code, codeFormat, labelCount, varName, varName, lineNum, labelCount);
    DS_appendString(dString, code);
    free(code);

    return 0;
}

int reType(Token *tokenArr, int isGlobal)
{
    if (tokenArr[0].type == TYPE_FLOAT)
    {
        DS_appendString(getCode(isGlobal), "PUSHS float@");
        char *float_string;
        formatString2string(float_string, "%a", tokenArr[0].attribute.doubleV);
        DS_appendString(getCode(isGlobal), float_string);
        free(float_string);
        DS_appendString(getCode(isGlobal), "\n");
    }
    else if (tokenArr[0].type == TYPE_INT)
    {
        DS_appendString(getCode(isGlobal), "PUSHS int@");
        char *int_string;
        formatString2string(int_string, "%d", tokenArr[0].attribute.intV);
        DS_appendString(getCode(isGlobal), int_string);
        free(int_string);
        DS_appendString(getCode(isGlobal), "\n");
    }
    if (tokenArr[2].type == TYPE_FLOAT)
    {
        DS_appendString(getCode(isGlobal), "PUSHS float@");
        char *float_string;
        formatString2string(float_string, "%a", tokenArr[2].attribute.doubleV);
        DS_appendString(getCode(isGlobal), float_string);
        free(float_string);
        DS_appendString(getCode(isGlobal), "\n");
    }
    else if (tokenArr[2].type == TYPE_INT)
    {
        DS_appendString(getCode(isGlobal), "PUSHS int@");
        char *int_string;
        formatString2string(int_string, "%d", tokenArr[2].attribute.intV);
        DS_appendString(getCode(isGlobal), int_string);
        free(int_string);
        DS_appendString(getCode(isGlobal), "\n");
    }
    return 0;
}

void CODEarithmetic(int ruleUsed, Token *tokenArr, Token endToken, int isGlobal)
{
    if (ruleUsed == 0)
    {
        // E => ID
        CODEcheckInitVar(getCode(isGlobal), tokenArr[0].attribute.dString->string, false, tokenArr[0].rowNumber);
        DS_appendString(getCode(isGlobal), "PUSHS LF@");
        DS_appendString(getCode(isGlobal), tokenArr[0].attribute.dString->string);
        DS_appendString(getCode(isGlobal), "\n");
    }

    if (ruleUsed == 1)
    {
        // E => INT
        DS_appendString(getCode(isGlobal), "PUSHS int@");
        char *int_string;
        formatString2string(int_string, "%d", tokenArr[0].attribute.intV);
        DS_appendString(getCode(isGlobal), int_string);
        DS_appendString(getCode(isGlobal), "\n");
        free(int_string);
    }

    if (ruleUsed == 2)
    {
        // E => FLOAT
        DS_appendString(getCode(isGlobal), "PUSHS float@");
        char *float_string;
        formatString2string(float_string, "%a", tokenArr[0].attribute.doubleV);
        DS_appendString(getCode(isGlobal), float_string);
        DS_appendString(getCode(isGlobal), "\n");
        free(float_string);
    }

    if (ruleUsed == 3)
    {
        // E => STRING
        DS_appendString(getCode(isGlobal), "PUSHS string@");
        DS_appendString(getCode(isGlobal), tokenArr[0].attribute.dString->string);
        DS_appendString(getCode(isGlobal), "\n");
    }

    if (ruleUsed == 4)
    {
        // E => FUNID
    }

    if (ruleUsed == 5)
    {
        // E => NULL
        DS_appendString(getCode(isGlobal), "PUSHS nil@nil\n");
    }

    if (ruleUsed > 5 && ruleUsed < 9)
    {
        // E => E + E
        static int counter = 0;
        counter++;
        char *type[3];
        char *instruction[5];

        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        switch (ruleUsed)
        {
        case 6:
            DS_appendString(getCode(isGlobal), "ADDS\n");
            break;
        case 7:
            DS_appendString(getCode(isGlobal), "SUBS\n");
            break;
        case 8:
            DS_appendString(getCode(isGlobal), "MULS\n");
            break;
        default:
            break;
        }

    }

    if (ruleUsed == 9)
    {
        // E => E / E
        static int counter = 0;
        counter++;

        DS_appendString(getCode(isGlobal), "\n#div\n");
        DS_appendString(getCode(isGlobal), "CREATEFRAME\n");

        reType(tokenArr, isGlobal);

        char *int_string;

        DS_appendString(getCode(isGlobal), "DEFVAR TF@a\n");
        DS_appendString(getCode(isGlobal), "DEFVAR TF@b\n");
        DS_appendString(getCode(isGlobal), "POPS TF@b\n");
        DS_appendString(getCode(isGlobal), "POPS TF@a\n");
        DS_appendString(getCode(isGlobal), "PUSHS TF@a\n");
        DS_appendString(getCode(isGlobal), "PUSHS TF@b\n");
        DS_appendString(getCode(isGlobal), "DEFVAR TF@atype\n");
        DS_appendString(getCode(isGlobal), "DEFVAR TF@btype\n");
        DS_appendString(getCode(isGlobal), "TYPE TF@atype TF@a\n");
        DS_appendString(getCode(isGlobal), "TYPE TF@btype TF@b\n");

        DS_appendString(getCode(isGlobal), "JUMPIFNEQ _afloatdiv");
        formatString2string(int_string, "%d", counter);
        DS_appendString(getCode(isGlobal), int_string);
        DS_appendString(getCode(isGlobal), " TF@atype string@int\n");

        DS_appendString(getCode(isGlobal), " POPS TF@b\n");
        DS_appendString(getCode(isGlobal), " INT2FLOATS\n");
        DS_appendString(getCode(isGlobal), " PUSHS TF@b\n");

        DS_appendString(getCode(isGlobal), "LABEL _afloatdiv");
        formatString2string(int_string, "%d", counter);
        DS_appendString(getCode(isGlobal), int_string);
        DS_appendString(getCode(isGlobal), "\n");

        DS_appendString(getCode(isGlobal), "JUMPIFNEQ _candiv");
        formatString2string(int_string, "%d", counter);
        DS_appendString(getCode(isGlobal), int_string);
        DS_appendString(getCode(isGlobal), " TF@btype string@int\n");

        DS_appendString(getCode(isGlobal), "INT2FLOATS\n");
        DS_appendString(getCode(isGlobal), "LABEL _candiv");
        formatString2string(int_string, "%d", counter);
        DS_appendString(getCode(isGlobal), int_string);
        DS_appendString(getCode(isGlobal), "\n");

        DS_appendString(getCode(isGlobal), "DIVS\n");
        DS_appendString(getCode(isGlobal), "CREATEFRAME\n");
    }

    if (ruleUsed == 10)
    {
        char *code ="\
CALL _convert2BiggestType\n\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@b\n\
DEFVAR TF@res\n\
POPS TF@a\n\
POPS TF@b\n\
CONCAT TF@res TF@b TF@a\n\
PUSHS TF@res\n\
        ";
        DS_appendString(getCode(isGlobal), code);
    }

    if (ruleUsed == 12)
    {
        // E => E === E

        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "EQS\n");
    }

    if (ruleUsed == 13)
    {
        // E => E !== E
        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "EQS\n");
        DS_appendString(getCode(isGlobal), "NOTS\n");
    }

    if (ruleUsed == 14)
    {
        // E => E < E
        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "LTS\n");
    }

    if (ruleUsed == 15)
    {
        // E => E > E
        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "GTS\n");
    }

    if (ruleUsed == 16)
    {
        // E => E <= E
        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "GTS\n");
        DS_appendString(getCode(isGlobal), "NOTS\n");
    }

    if (ruleUsed == 17)
    {
        // E => E >=  E
        DS_appendString(getCode(isGlobal), "CALL _convert2BiggestType\n");
        DS_appendString(getCode(isGlobal), "LTS\n");
        DS_appendString(getCode(isGlobal), "NOTS\n");
    }
}

// read, write + zadani str. 10, udelat: ulozit do symtable, generovat kod
int CODEbuiltInFunc(DynamicString *dString)
{
    char *code = "\
.IFJcode22\n\
JUMP _main\n\n";
    DS_appendString(dString, code);

    // write
    code = "\
######WRITE######\n\
LABEL write\n\
TYPE LF@writetype LF@tmpwrite\n\
JUMPIFEQ _writeempty LF@writetype string@nil\n\
WRITE LF@tmpwrite\n\
POPFRAME\n\
CREATEFRAME\n\
RETURN\n\
#EMPTY#\n\
LABEL _writeempty\n\
WRITE string@\n\
POPFRAME\n\
CREATEFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

    // readi
    code = "\
######READI######\n\
LABEL readi\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input int\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readiOk TF@input$type string@int\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readiOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

    // readf
    code = "\
######READF######\n\
LABEL readf\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input float\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readfOk TF@input$type string@float\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readfOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//reads
code = "\
######READS######\n\
LABEL reads\n\
CREATEFRAME\n\
DEFVAR TF@input\n\
DEFVAR TF@input$type\n\
READ TF@input string\n\
TYPE TF@input$type TF@input\n\
JUMPIFEQ _readsOk TF@input$type string@string\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _readsOk\n\
PUSHS TF@input\n\
CREATEFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//floatval
code = "\
######FLOATVAL######\n\
LABEL floatval\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@floatval\n\
POPS LF@floatval\n\
";
DS_appendString(dString, code);
CODEconvert2Type(dString,"LF@floatval",'f'); // conversion to float
code = "\
PUSHS LF@floatval\n\
POPFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

//intval
code = "\
######INTVAL######\n\
LABEL intval\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@intval\n\
POPS LF@intval\n\
";
DS_appendString(dString, code);
CODEconvert2Type(dString,"LF@intval",'i'); // conversion to int
code = "\
PUSHS LF@intval\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//strval
code = "\
######STRVAL######\n\
LABEL strval\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@strval\n\
POPS LF@strval\n\
";
DS_appendString(dString, code);
CODEconvert2Type(dString,"LF@strval",'s'); // conversion to string
code = "\
PUSHS LF@strval\n\
POPFRAME\n\
RETURN\n\
\n";
DS_appendString(dString, code);

//strlen
code = "\
######STRLEN######\n\
LABEL strlen\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@string\n\
POPS LF@string\n\
DEFVAR LF@strlen\n\
STRLEN LF@strlen LF@string\n\
PUSHS LF@strlen\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//substring
code = "\
######SUBSTRING######\n\
LABEL substring\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@j\n\
POPS LF@j\n\
DEFVAR LF@i\n\
POPS LF@i\n\
DEFVAR LF@string\n\
POPS LF@string\n\
DEFVAR LF@done\n\
\
#Check NULL return\n\
LT LF@done LF@i int@0\n\
JUMPIFEQ _substringIsNull LF@done bool@true\n\
LT LF@done LF@j int@0\n\
JUMPIFEQ _substringIsNull LF@done bool@true\n\
GT LF@done LF@i LF@j\n\
JUMPIFEQ _substringIsNull LF@done bool@true\n\
DEFVAR LF@stringlen\n\
STRLEN LF@stringlen LF@string\n\
LT LF@done LF@i LF@stringlen\n\
JUMPIFNEQ _substringIsNull LF@done bool@true #NOT i < strlen\n\
GT LF@done LF@j LF@stringlen\n\
JUMPIFEQ _substringIsNull LF@done bool@true\n\
JUMP _substringNotNull\n\
\
LABEL _substringIsNull\n\
PUSHS nil@nil\n\
POPFRAME\n\
RETURN\n\
LABEL _substringNotNull\n\
\
#Create substring\n\
DEFVAR LF@retString\n\
MOVE LF@retString string@\n\
DEFVAR LF@curChar\n\
LABEL _substringNextChar #While i < j\n\
LT LF@done LF@i LF@j\n\
JUMPIFNEQ _substringEnd LF@done bool@true # NOT i < j\n\
GETCHAR LF@curChar LF@string LF@i\n\
CONCAT LF@retString LF@retString LF@curChar\n\
ADD LF@i LF@i int@1\n\
JUMP _substringNextChar\n\
LABEL _substringEnd\n\
\
PUSHS LF@retString\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//ord
code = "\
######ORD######\n\
LABEL ord\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@ordV\n\
POPS LF@ordV\n\
JUMPIFNEQ _%%ordRetChar LF@ordV string@\n\
PUSHS int@0\n\
RETURN\n\
LABEL _%%ordRetChar\n\
STRI2INT LF@ordV LF@ordV int@0\n\
PUSHS LF@ordV\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//chr
code = "\
######CHR######\n\
LABEL chr\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@chrV\n\
POPS LF@chrV\n\
INT2CHAR LF@chrV LF@chrV\n\
PUSHS LF@chrV\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//convert2BiggestType
code = "\
######convert2BiggestType######\n\
LABEL _convert2BiggestType\n\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@b\n\
POPS TF@b\n\
POPS TF@a\n\
DEFVAR TF@atype\n\
DEFVAR TF@btype\n\
\
TYPE TF@atype TF@a\n\
TYPE TF@btype TF@b\n\
JUMPIFNEQ _aNotNil TF@atype string@nil\n\
MOVE TF@a int@0\n\
TYPE TF@atype TF@a\n\
JUMPIFNEQ _aNotNil TF@btype string@string\n\
MOVE TF@a string@\n\
TYPE TF@atype TF@a\n\
LABEL _aNotNil\n\
\
TYPE TF@btype TF@b\n\
JUMPIFNEQ _bNotNil TF@btype string@nil\n\
MOVE TF@b int@0\n\
TYPE TF@btype TF@b\n\
JUMPIFNEQ _bNotNil TF@atype string@string\n\
MOVE TF@b string@\n\
TYPE TF@btype TF@b\n\
LABEL _bNotNil\n\
\
JUMPIFEQ _sameType TF@atype TF@btype\n\
JUMPIFEQ _convToString TF@atype string@string\n\
JUMPIFEQ _convToString TF@btype string@string\n\
JUMPIFEQ _bConvToFloat TF@atype string@float\n\
JUMPIFEQ _aConvToFloat TF@btype string@float\n\
LABEL _convToString\n\
DPRINT string@Implicit\\032convesion\\032to\\032string\\032not\\032supported!\\010\n\
EXIT int@7\n\
JUMP _sameType\n\
LABEL _aConvToFloat\n\
INT2FLOAT TF@a TF@a\n\
JUMP _sameType\n\
\
LABEL _bConvToFloat\n\
INT2FLOAT TF@b TF@b\n\
JUMP _sameType\n\
\
LABEL _sameType\n\
PUSHS TF@a\n\
PUSHS TF@b\n\
CREATEFRAME\n\
RETURN\n";
DS_appendString(dString, code);

    return 0;
}