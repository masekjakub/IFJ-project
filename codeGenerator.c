/**
 * @file codeGenerator.c
 * @authors Martin Zelenák, Jakub Mašek, Vojta Kuchař
 * @brief Functions for generating large chunks of IFJcode22 code
 * @version 1.0
 * @date 2022-11-06
 */

#include "codeGenerator.h"

/**
 * @brief Converts given format string into string.
 * Resulting string is malloced and its ptr saved into DEST.
 * DEST should be freed, when not needed.
 * @param DEST Variable name of type char *, where the resulting string should be stored
 * @param FORMAT Format string to be converted
 * @param FORMAT_ARGS Format string arguments (0...n)
 */
#define formatString2string(DEST, FORMAT, FORMAT_ARGS...)                       \
    DEST = malloc((snprintf(NULL, 0, FORMAT, FORMAT_ARGS) + 1) * sizeof(char)); \
    if (DEST == NULL)                                                           \
        exit(ERR_INTERN);                                                       \
    sprintf(DEST, FORMAT, FORMAT_ARGS);
#define isLower(CHAR) ('a' <= CHAR && CHAR <= 'z')

/**
 * @brief Generates code for start of main
 *
 * @param dString Dynamic string to append the code to
 */
void CODEmain(DynamicString *dString)
{
    DS_appendString(dString, "######MAIN######\n");
    DS_appendString(dString, "LABEL _&main\n");
    DS_appendString(dString, "DEFVAR GF@void\n");
    DS_appendString(dString, "CREATEFRAME\n");
    DS_appendString(dString, "PUSHFRAME\n");
}

/**
 * @brief Checks the type of given variable and
 * converts it ot given type indicated by char type.
 * @param varName Name of varible to convert (name includes LF@/GF@)
 * Creates its new TF -> can't convert TF@* variables
 * @param dString Dynamic string to append the code to
 * @param type Char representing the type to convert to:
 * 'i'=int ,'b'=bool, 'f'=float, 's'=string, 'n'=nil
 */
void CODEconvert2Type(DynamicString *dString, char *varName, char type)
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

    //Conversion to given type
    switch (type)
    {
    case 'i':
        codeFormat = "\
JUMPIFEQ _&convFloat2Int%d TF@%%convType%d string@float\n\
JUMPIFEQ _&convNil2Int%d TF@%%convType%d string@nil\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convFloat2Int%d\n\
FLOAT2INT %s %s\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convNil2Int%d\n\
MOVE %s int@0\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName

        code = NULL;
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 'b':
        codeFormat = "\
JUMPIFEQ _&convInt2Bool%d TF@%%convType%d string@int\n\
JUMPIFEQ _&convFloat2Bool%d TF@%%convType%d string@float\n\
JUMPIFEQ _&convString2Bool%d TF@%%convType%d string@string\n\
JUMPIFEQ _&convNil2Bool%d TF@%%convType%d string@nil\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convInt2Bool%d\n\
JUMPIFNEQ _&convInt2Bool%d_true %s int@0\n\
MOVE %s bool@false\n\
JUMP _&noConv%d\n\
LABEL _&convInt2Bool%d_true\n\
MOVE %s bool@true\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convFloat2Bool%d\n\
JUMPIFNEQ _&convFloat2Bool%d_true %s float@0x0p+0\n\
MOVE %s bool@false\n\
JUMP _&noConv%d\n\
LABEL _&convFloat2Bool%d_true\n\
MOVE %s bool@true\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convString2Bool%d\n\
JUMPIFEQ _&convString2Bool%d_false %s string@\n\
JUMPIFEQ _&convString2Bool%d_false %s string@0\n\
MOVE %s bool@true\n\
JUMP _&noConv%d\n\
LABEL _&convString2Bool%d_false\n\
MOVE %s bool@false\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convNil2Bool%d\n\
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
JUMPIFEQ _&convInt2Float%d TF@%%convType%d string@int\n\
JUMPIFEQ _&convNil2Float%d TF@%%convType%d string@nil\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convInt2Float%d\n\
INT2FLOAT %s %s\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convNil2Float%d\n\
MOVE %s float@0x0p+0\n\
"; //, varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varNameNum,varName,varName,varNameNum,varNameNum,varName
        code = NULL;
        formatString2string(code, codeFormat, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varNameNum, varName, varName, varNameNum, varNameNum, varName);
        DS_appendString(dString, code);
        free(code);
        break;

    case 's':
        codeFormat = "\
JUMPIFEQ _&convNil2String%d TF@%%convType%d string@nil\n\
JUMP _&noConv%d\n\
\n\
LABEL _&convNil2String%d\n\
MOVE %s string@\n\
JUMP _&noConv%d\n\
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

    //End label
    codeFormat = "\
LABEL _&noConv%d\n\n\
"; //, varNameNum

    code = NULL;
    formatString2string(code, codeFormat, varNameNum);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for if statement header
 * Expects the condition value on stack
 * @param dStringPtr Ptr to ptr to dynamic string to append the code to
 * @param ifCount Number used for unique labels
 * @param lastUnconditionedLine Index of last line in dString outside
 * any 'if' or 'while' code block.
 * -1 = not inside conditioned code block
 */
void CODEifStart(DynamicString **dStringPtr, int ifCount, int lastUnconditionedLine)
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
JUMPIFEQ _&else%d LF@%%if_cond%d bool@false\n\
"; //, ifCount,ifCount

    code = NULL;
    formatString2string(code, codeFormat, ifCount, ifCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for if statement else header
 * 
 * @param dString Dynamic string to append the code to
 * @param ifCount Number used for unique labels
 */
void CODEelse(DynamicString *dString, int ifCount)
{
    char *codeFormat = "\
#CODEelse\n\
JUMP _&endif%d\n\
LABEL _&else%d\n\
";
    char *code = NULL;
    formatString2string(code, codeFormat, ifCount, ifCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for if statement end
 * 
 * @param dStringPtr Dynamic string to append the code to
 * @param ifCount Number used for unique labels
 */
void CODEifEnd(DynamicString *dString, int ifCount)
{
    char *codeFormat = "\
#CODEifEnd\n\
LABEL _&endif%d\n\
";
    char *code = NULL;
    formatString2string(code, codeFormat, ifCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for while statement header
 * 
 * @param dStringPtr Ptr to ptr to dynamic string to append the code to
 * @param whileCount Number used for unique labels
 * @param lastUnconditionedLine Index of last line in dString outside
 * any 'if' or 'while' code block.
 * -1 = not inside conditioned code block
 */
void CODEwhileStart(DynamicString **dStringPtr, int whileCount, int lastUnconditionedLine)
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
LABEL _&whileStart%d\n\
"; //, whileCount

    formatString2string(code, codeFormat, whileCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for while statement conditioned start.
 * Expects the condition value on stack.
 * @param dString Dynamic string to append the code to
 * @param whileCount Number used for unique labels
 */
void CODEwhileCond(DynamicString *dString, int whileCount)
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
JUMPIFEQ _&whileEnd%d LF@%%while_cond%d bool@false\n\
"; //, whileCount,whileCount

    code = NULL;
    formatString2string(code, codeFormat, whileCount, whileCount);
    DS_appendString(dString, code);
    free(code);

    
}

/**
 * @brief Generates code for while statement end
 * 
 * @param dString Dynamic string to append the code to
 * @param whileCount Number used for unique labels
 */
void CODEwhileEnd(DynamicString *dString, int whileCount)
{
    char *code = NULL;
    char *codeFormat = "\
JUMP _&whileStart%d\n\
LABEL _&whileEnd%d\n\
"; //, whileCount,whileCount

    formatString2string(code, codeFormat, whileCount, whileCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for definition of function parametr
 * 
 * @param dString Dynamic string to append the code to
 * @param paramName Name of parametr to be defined
 */
void CODEparam(DynamicString *dString, char *paramName)
{
    char *codeFormat = "\
#CODEparam\n\
DEFVAR LF@%s\n\
"; //, paramName

    char *code = NULL;
    formatString2string(code, codeFormat, paramName);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for popping value of 
 * function parametr from stack and checking its type.
 * @param dString Dynamic string to append the code to
 * @param paramName Name of parametr to assign the value to
 * @param paramType Which type should the parametr value have
 */
void CODEpopParam(DynamicString *dString, char *paramName, char paramType)
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

    // Check parametr value type
    if(!isLower(paramType)){
        codeFormat = "JUMPIFEQ _&popParamRightType%d TF@paramType string@nil\n"; //, popValueCount
        formatString2string(code, codeFormat, popValueCount);
        DS_appendString(dString, code);
        free(code);

        paramType += 32; //Convert to lowercase
    }
    switch (paramType)
    {
    case 'i':
        codeFormat = "JUMPIFEQ _&popParamRightType%d TF@paramType string@int\n"; //, popValueCount
        break;
    case 'f':
        codeFormat = "JUMPIFEQ _&popParamRightType%d TF@paramType string@float\n"; //, popValueCount
        break;
    case 's':
        codeFormat = "JUMPIFEQ _&popParamRightType%d TF@paramType string@string\n"; //, popValueCount
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
LABEL _&popParamRightType%d\n\
"; //, paramName,popValueCount

    formatString2string(code, codeFormat, paramName,popValueCount);
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for function definition header
 * 
 * @param dString Dynamic string to append the code to
 * @param functionName Name of defined function
 */
void CODEfuncDef(DynamicString *dString, char *functionName)
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
}

/**
 * @brief Generates code for function return.
 * Checks return value based on given returnType.
 * @param dString Dynamic string to append the code to
 * @param returnType 1st char of type name the function should return.
 * ('i','f','s')
 * @param lineNum 
 */
void CODEfuncReturn(DynamicString *dString, char returnType, int lineNum)
{
    static int returnNum = 0;
    returnNum++;

    char *codeFormat = NULL;
    char *code = "\
#CODEreturn\n\
CREATEFRAME\n\
DEFVAR TF@retVal\n\
POPS TF@retVal\n\
DEFVAR TF@retValType\n\
TYPE TF@retValType TF@retVal\n\
";
    DS_appendString(dString, code);

    // Check if function can return null(nil)
    if (!isLower(returnType))
    {
        codeFormat = "\
JUMPIFEQ _&rightReturnType%d TF@retValType string@nil\n\
";    //, returnNum
        code = NULL;
        formatString2string(code, codeFormat, returnNum);
        DS_appendString(dString, code);
        free(code);
        returnType += 32; // To lower case
    }

    // Type control
    switch (returnType)
    {
    case 'i':
        codeFormat = "\
JUMPIFEQ _&rightReturnType%d TF@retValType string@int\n\
"; //, returnNum
        break;
    case 'f':
        codeFormat = "\
JUMPIFEQ _&rightReturnType%d TF@retValType string@float\n\
"; //, returnNum
        break;
    case 's':
        codeFormat = "\
JUMPIFEQ _&rightReturnType%d TF@retValType string@string\n\
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

    codeFormat = "\
DPRINT string@Wrong\\032return\\032type\\032on\\032line\\032%d\\033\\010\n\
EXIT int@4\n\
LABEL _&rightReturnType%d\n\
PUSHS TF@retVal\n\
POPFRAME\n\
CREATEFRAME\n\
RETURN\n\
"; //, lineNum,returnNum

    code = NULL;
    formatString2string(code, codeFormat, lineNum, returnNum);
    DS_appendString(dString, code);
    free(code);    
}

/**
 * @brief Generates code for function definition end
 * based on given isVoid.
 * @param dString Dynamic string to append the code to
 * @param funId Function name
 * @param isVoid If function can return null(nil)
 */
void CODEfuncDefEnd(DynamicString *dString, char *funId, bool isVoid)
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
}

/**
 * @brief Generates code for calling write function 
 * with arguments in the right order.
 * @param dString Dynamic string to append the code to
 * @param argCount Number of arguments
 */
void CODEcallWrite(DynamicString *dString, int argCount)
{
    if(argCount == 0){
        DS_appendString(dString, "PUSHS nil@nil\n");
        return;
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
        return;
    }
}

/**
 * @brief Generates code for function call
 *
 * @param dString Dynamic string to append the code to
 * @param token functionID token
 * @param argCount Number of arguments
 */
void CODEfuncCall(DynamicString *dString, Token token, int argCount)
{
    if (!strcmp(DS_string(token.attribute.dString), "write"))
    {
        CODEcallWrite(dString, argCount);
        return;
    }
    char *code = NULL;
    char *codeFormat = "\
#CODEfuncCall\n\
CALL %s\n\
";
    formatString2string(code, codeFormat, DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief defines new var on last unconditioned line (always performed)
 *
 * @param dString ptr of ptr to string to save in
 * @param varName Name of variable to be defined
 * @param lastUnconditionedLine
 * Index in dString, where definition of variable should be generated.
 * -1 = code will be appended
 */
void CODEdefVar(DynamicString **dStringPtr, char *varName, int lastUnconditionedLine)
{
    char *code = NULL;
    char *codeFormat = "\
#CODEdefVar\n\
DEFVAR LF@%s\n\
"; //, varName
    formatString2string(code, codeFormat, varName);
    // Check where should the definition be generated
    if (lastUnconditionedLine == -1)
    {
        DS_appendString(*dStringPtr, code);
    }
    else
    {
        DS_insertString(dStringPtr, code, lastUnconditionedLine);
    }
    free(code);
}

/**
 * @brief Generates code for assigning a value 
 * from stack to a given local variable
 * @param dString Dynamic string to append the code to
 * @param token Token of variable to assign the value to
 */
void CODEassign(DynamicString *dString, Token token)
{
    char *code = NULL;
    char *codeFormat = "\
#CODEassign\n\
POPS LF@%s\n";
    formatString2string(code, codeFormat, DS_string(token.attribute.dString));
    DS_appendString(dString, code);
    free(code);
}

/**
 * @brief Generates code for checking initialization of variable.
 * 
 * @param dString Dynamic string to append the code to
 * @param varName Name of variable
 * @param isGlobalFrame Determines if var is global or local
 * @param lineNum Current line number (used for error message)
 */
void CODEcheckInitVar(DynamicString *dString, char *varName, bool isGlobalFrame, int lineNum)
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

    // Get type of given variable (if uninit -> type="")
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

    // Check initialization
    codeFormat = "\
JUMPIFNEQ _&varInitOk%d TF@%sType string@\n\
DPRINT string@Variable\\032%s\\032is\\032undefined\\032on\\032line\\032%d!\\010\n\
EXIT int@5\n\
LABEL _&varInitOk%d\n\
CREATEFRAME\n\
"; //, labelCount,varName,varName,lineNum,labelCount
    formatString2string(code, codeFormat, labelCount, varName, varName, lineNum, labelCount);
    DS_appendString(dString, code);
    free(code);
}


/**
 * @brief Generates code for operators in expressions
 * 
 * @param ruleUsed Expression rule used in expression (represents operator)
 * @param tokenArr Array of 3 tokens used in operation (operand, operator, operand)
 * @param isGlobal Determines what code string to append to
 */
//udelat: předělat isGlobal na DynamicString *dString
void CODEarithmetic(DynamicString *dString, int ruleUsed, Token *tokenArr)
{
    if (ruleUsed == 0)
    {
        // E => ID
        CODEcheckInitVar(dString, tokenArr[0].attribute.dString->string, false, tokenArr[0].rowNumber);
        DS_appendString(dString, "PUSHS LF@");
        DS_appendString(dString, tokenArr[0].attribute.dString->string);
        DS_appendString(dString, "\n");
    }

    if (ruleUsed == 1)
    {
        // E => INT
        DS_appendString(dString, "PUSHS int@");
        char *int_string;
        formatString2string(int_string, "%d", tokenArr[0].attribute.intV);
        DS_appendString(dString, int_string);
        DS_appendString(dString, "\n");
        free(int_string);
    }

    if (ruleUsed == 2)
    {
        // E => FLOAT
        DS_appendString(dString, "PUSHS float@");
        char *float_string;
        formatString2string(float_string, "%a", tokenArr[0].attribute.doubleV);
        DS_appendString(dString, float_string);
        DS_appendString(dString, "\n");
        free(float_string);
    }

    if (ruleUsed == 3)
    {
        // E => STRING
        DS_appendString(dString, "PUSHS string@");
        DS_appendString(dString, tokenArr[0].attribute.dString->string);
        DS_appendString(dString, "\n");
    }

    if (ruleUsed == 4)
    {
        // E => FUNID
    }

    if (ruleUsed == 5)
    {
        // E => NULL
        DS_appendString(dString, "PUSHS nil@nil\n");
    }

    if (ruleUsed > 5 && ruleUsed < 10)
    {
        DS_appendString(dString, "CALL _&convert2BiggestType\n");
        DS_appendString(dString, "CALL _&isValue\n");
        switch (ruleUsed)
        {
        case 6:
            DS_appendString(dString, "ADDS\n");
            break;
        case 7:
            DS_appendString(dString, "SUBS\n");
            break;
        case 8:
            DS_appendString(dString, "MULS\n");
            break;
        case 9:
            DS_appendString(dString, "CREATEFRAME\n");
            DS_appendString(dString, "DEFVAR TF@a\n");
            DS_appendString(dString, "DEFVAR TF@b\n");
            DS_appendString(dString, "POPS TF@b\n");
            DS_appendString(dString, "POPS TF@a\n");
            DS_appendString(dString, "PUSHFRAME\n");
            CODEconvert2Type(dString,"LF@a",'f');
            CODEconvert2Type(dString,"LF@b",'f');
            DS_appendString(dString, "PUSHS LF@a\n");
            DS_appendString(dString, "PUSHS LF@b\n");
            DS_appendString(dString, "DIVS\n");
            DS_appendString(dString, "POPFRAME\n");
            break;
        default:
            break;
        }
    }

    if (ruleUsed == 10)
    {
    char *code ="\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@b\n\
POPS TF@b\n\
POPS TF@a\n\
PUSHFRAME\n";
    DS_appendString(dString, code);
    CODEconvert2Type(dString,"LF@a",'s');
    CODEconvert2Type(dString,"LF@b",'s');
    code ="\
PUSHS LF@a\n\
PUSHS LF@b\n\
CALL _&isString\n\
POPFRAME\n\
DEFVAR TF@res\n\
CONCAT TF@res TF@a TF@b\n\
PUSHS TF@res\n";
    DS_appendString(dString, code);
    }

    if (ruleUsed == 12)
    {
        // E => E === E
        DS_appendString(dString, "CALL _&isSameType\n");
        DS_appendString(dString, "EQS\n");
    }

    if (ruleUsed == 13)
    {
        // E => E !== E
        DS_appendString(dString, "CALL _&isSameType\n");
        DS_appendString(dString, "EQS\n");
        DS_appendString(dString, "NOTS\n");
    }

    if (ruleUsed == 14)
    {
        // E => E < E
        DS_appendString(dString, "CALL _&convert2BiggestType\n");
        DS_appendString(dString, "LTS\n");
    }

    if (ruleUsed == 15)
    {
        // E => E > E
        DS_appendString(dString, "CALL _&convert2BiggestType\n");
        DS_appendString(dString, "GTS\n");
    }

    if (ruleUsed == 16)
    {
        // E => E <= E
        DS_appendString(dString, "CALL _&convert2BiggestType\n");
        DS_appendString(dString, "GTS\n");
        DS_appendString(dString, "NOTS\n");
    }

    if (ruleUsed == 17)
    {
        // E => E >=  E
        DS_appendString(dString, "CALL _&convert2BiggestType\n");
        DS_appendString(dString, "LTS\n");
        DS_appendString(dString, "NOTS\n");
    }
}

/**
 * @brief Generates code for built-in functions and utility functions. \n
 * Built-in: write, readi, readf, reads, floatval, intval, strval, strlen, substring, ord, chr
 * Utility: _&convert2BiggestType, _&isString, _&isValue, _&isSameType
 * @param dString Dynamic string to append the code to
 */
void CODEbuiltInFunc(DynamicString *dString)
{
char *code = "\
.IFJcode22\n\
JUMP _&main\n\n";
    DS_appendString(dString, code);

// write
code = "\
######WRITE######\n\
LABEL write\n\
TYPE LF@writetype LF@tmpwrite\n\
JUMPIFEQ _&writeempty LF@writetype string@nil\n\
WRITE LF@tmpwrite\n\
POPFRAME\n\
CREATEFRAME\n\
RETURN\n\
#EMPTY#\n\
LABEL _&writeempty\n\
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
JUMPIFEQ _&readiOk TF@input$type string@int\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&readiOk\n\
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
JUMPIFEQ _&readfOk TF@input$type string@float\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&readfOk\n\
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
JUMPIFEQ _&readsOk TF@input$type string@string\n\
PUSHS nil@nil\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&readsOk\n\
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
DEFVAR LF@strvalType\n\
TYPE LF@strvalType LF@strval\n\
JUMPIFEQ _&strvalTypeOk LF@strvalType string@string\n\
JUMPIFEQ _&strvalTypeOk LF@strvalType string@nil\n\
DPRINT string@Only\\032arguments\\032of\\032type\\032null\\032or\\032string\\032are\\032supported\\032by\\032strval!\n\
EXIT int@4\n\
LABEL _&strvalTypeOk\n\
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
DEFVAR LF@s\n\
POPS LF@s\n\
DEFVAR LF@stringType\n\
TYPE LF@stringType LF@s\n\
JUMPIFEQ _&strlenTypeOk LF@stringType string@string\n\
DPRINT string@Strlen\\032argument\\032is\\032not\\032of\\032type\\032string!\n\
EXIT int@4\n\
LABEL _&strlenTypeOk\n\
\
DEFVAR LF@strlen\n\
STRLEN LF@strlen LF@s\n\
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
DEFVAR LF@s\n\
POPS LF@s\n\
DEFVAR LF@done\n\
\
#Check param types\n\
DEFVAR LF@paramType\n\
TYPE LF@paramType LF@s\n\
JUMPIFEQ _&substringStringTypeOk LF@paramType string@string\n\
DPRINT string@Substring\\032argument\\032$s\\032is\\032not\\032of\\032type\\032string!\n\
EXIT int@4\n\
LABEL _&substringStringTypeOk\n\
TYPE LF@paramType LF@i\n\
JUMPIFEQ _&substringIndexITypeOk LF@paramType string@int\n\
DPRINT string@Substring\\032argument\\032$i\\032is\\032not\\032of\\032type\\032int!\n\
EXIT int@4\n\
LABEL _&substringIndexITypeOk\n\
TYPE LF@paramType LF@j\n\
JUMPIFEQ _&substringIndexJTypeOk LF@paramType string@int\n\
DPRINT string@Substring\\032argument\\032$j\\032is\\032not\\032of\\032type\\032int!\n\
EXIT int@4\n\
LABEL _&substringIndexJTypeOk\n\
\
#Check NULL return\n\
LT LF@done LF@i int@0\n\
JUMPIFEQ _&substringIsNull LF@done bool@true\n\
LT LF@done LF@j int@0\n\
JUMPIFEQ _&substringIsNull LF@done bool@true\n\
GT LF@done LF@i LF@j\n\
JUMPIFEQ _&substringIsNull LF@done bool@true\n\
DEFVAR LF@stringlen\n\
STRLEN LF@stringlen LF@s\n\
LT LF@done LF@i LF@stringlen\n\
JUMPIFNEQ _&substringIsNull LF@done bool@true #NOT i < strlen\n\
GT LF@done LF@j LF@stringlen\n\
JUMPIFEQ _&substringIsNull LF@done bool@true\n\
JUMP _&substringNotNull\n\
\
LABEL _&substringIsNull\n\
PUSHS nil@nil\n\
POPFRAME\n\
RETURN\n\
LABEL _&substringNotNull\n\
\
#Create substring\n\
DEFVAR LF@retString\n\
MOVE LF@retString string@\n\
DEFVAR LF@curChar\n\
LABEL _&substringNextChar #While i < j\n\
LT LF@done LF@i LF@j\n\
JUMPIFNEQ _&substringEnd LF@done bool@true # NOT i < j\n\
GETCHAR LF@curChar LF@s LF@i\n\
CONCAT LF@retString LF@retString LF@curChar\n\
ADD LF@i LF@i int@1\n\
JUMP _&substringNextChar\n\
LABEL _&substringEnd\n\
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
DEFVAR LF@c\n\
POPS LF@c\n\
DEFVAR LF@cType\n\
TYPE LF@cType LF@c\n\
JUMPIFEQ _&ordCTypeOk LF@cType string@string\n\
DPRINT string@Ord\\032argument\\032is\\032not\\032of\\032type\\032string!\n\
EXIT int@4\n\
LABEL _&ordCTypeOk\n\
JUMPIFNEQ _&%%ordRetChar LF@c string@\n\
PUSHS int@0\n\
POPFRAME\n\
RETURN\n\
LABEL _&%%ordRetChar\n\
STRI2INT LF@c LF@c int@0\n\
PUSHS LF@c\n\
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
DEFVAR LF@i\n\
POPS LF@i\n\
DEFVAR LF@iType\n\
TYPE LF@iType LF@i\n\
JUMPIFEQ _&chrITypeOk LF@iType string@int\n\
DPRINT string@Chr\\032argument\\032is\\032not\\032of\\032type\\032int!\n\
EXIT int@4\n\
LABEL _&chrITypeOk\n\
INT2CHAR LF@i LF@i\n\
PUSHS LF@i\n\
POPFRAME\n\
RETURN\n\
\n";
    DS_appendString(dString, code);

//_convert2BiggestType
code = "\
######convert2BiggestType######\n\
LABEL _&convert2BiggestType\n\
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
JUMPIFNEQ _&aNotNil TF@atype string@nil\n\
MOVE TF@a int@0\n\
TYPE TF@atype TF@a\n\
JUMPIFNEQ _&aNotNil TF@btype string@string\n\
MOVE TF@a string@\n\
TYPE TF@atype TF@a\n\
LABEL _&aNotNil\n\
\
TYPE TF@btype TF@b\n\
JUMPIFNEQ _&bNotNil TF@btype string@nil\n\
MOVE TF@b int@0\n\
TYPE TF@btype TF@b\n\
JUMPIFNEQ _&bNotNil TF@atype string@string\n\
MOVE TF@b string@\n\
TYPE TF@btype TF@b\n\
LABEL _&bNotNil\n\
\
JUMPIFEQ _&sameType TF@atype TF@btype\n\
JUMPIFEQ _&convToString TF@atype string@string\n\
JUMPIFEQ _&convToString TF@btype string@string\n\
JUMPIFEQ _&bConvToFloat TF@atype string@float\n\
JUMPIFEQ _&aConvToFloat TF@btype string@float\n\
LABEL _&convToString\n\
DPRINT string@Implicit\\032convesion\\032to\\032string\\032not\\032supported!\\010\n\
EXIT int@7\n\
JUMP _&sameType\n\
LABEL _&aConvToFloat\n\
INT2FLOAT TF@a TF@a\n\
JUMP _&sameType\n\
\
LABEL _&bConvToFloat\n\
INT2FLOAT TF@b TF@b\n\
JUMP _&sameType\n\
\
LABEL _&sameType\n\
PUSHS TF@a\n\
PUSHS TF@b\n\
CREATEFRAME\n\
RETURN\n";
    DS_appendString(dString, code);

//_isString
code = "\
######isString######\n\
LABEL _&isString\n\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@aType\n\
POPS TF@a\n\
TYPE TF@aType TF@a\n\
JUMPIFNEQ _&isStringRetFalse TF@aType string@string\n\
PUSHS TF@a\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&isStringRetFalse\n\
DPRINT string@Operand\\032has\\032to\\032be\\032a\\032string!\\010\n\
EXIT int@7\n\
CREATEFRAME\n\
RETURN\n";
    DS_appendString(dString, code);

//_isValue
code = "\
######isValue######\n\
LABEL _&isValue\n\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@aType\n\
POPS TF@a\n\
TYPE TF@aType TF@a\n\
JUMPIFEQ _&isValueRetFalse TF@aType string@string\n\
PUSHS TF@a\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&isValueRetFalse\n\
DPRINT string@Operand\\032has\\032to\\032be\\032a\\032value!\\010\n\
EXIT int@7\n\
CREATEFRAME\n\
RETURN\n";
    DS_appendString(dString, code);

//_isSameType
code = "\
######isSameType######\n\
LABEL _&isSameType\n\
CREATEFRAME\n\
DEFVAR TF@a\n\
DEFVAR TF@b\n\
DEFVAR TF@aType\n\
DEFVAR TF@bType\n\
POPS TF@a\n\
POPS TF@b\n\
TYPE TF@aType TF@a\n\
TYPE TF@bType TF@b\n\
JUMPIFNEQ _&isNotSameType TF@aType TF@bType\n\
PUSHS TF@b\n\
PUSHS TF@a\n\
CREATEFRAME\n\
RETURN\n\
LABEL _&isNotSameType\n\
PUSHS bool@true\n\
PUSHS bool@false\n\
CREATEFRAME\n\
RETURN\n";
    DS_appendString(dString, code);
}