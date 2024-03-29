/**
 * @file parser.c
 * @authors Jakub Mašek, Martin Zelenák
 * @brief Parser for IFJ22 compiler
 * @version 1.0
 * @date 2022-12-6
 */

#include "parser.h"
#define isKeyword(TOKEN, KEYWORD) TOKEN.attribute.keyword == KEYWORD
#define isValueType(TYPE) (TYPE == TYPE_INT || TYPE == TYPE_FLOAT || TYPE == TYPE_STRING || TYPE == TYPE_FUNID || TYPE == TYPE_ID || TYPE == TYPE_NULL)
#define isOperatorType(TYPE) (TYPE == TYPE_ADD || TYPE == TYPE_SUB || TYPE == TYPE_MUL || TYPE == TYPE_DIV || TYPE == TYPE_MOD || TYPE == TYPE_EQTYPES || TYPE == TYPE_NOTEQTYPES || TYPE == TYPE_LESS || TYPE == TYPE_GREATER || TYPE == TYPE_LESSEQ || \
                              TYPE == TYPE_GREATEREQ || TYPE == TYPE_CONCAT)
#define isBracket(TYPE) (TYPE == TYPE_LBRACKET || TYPE == TYPE_RBRACKET)
#define isKeywordType(KEYWORD) (KEYWORD == KEYWORD_INT || KEYWORD == KEYWORD_FLOAT || KEYWORD == KEYWORD_STRING || KEYWORD == KEYWORD_VOID)
#define isLower(CHAR) ('a' <= CHAR && CHAR <= 'z')
#define formatString2string(DEST, FORMAT, FORMAT_ARGS...)                       \
    DEST = malloc((snprintf(NULL, 0, FORMAT, FORMAT_ARGS) + 1) * sizeof(char)); \
    if (DEST == NULL)                                                           \
        exit(ERR_INTERN);                                                       \
    sprintf(DEST, FORMAT, FORMAT_ARGS);

int firstError;               // First encountered error
Symtable *globalST;           // Global symtable
Symtable *localST;            // Local symtable
int isGlobal;                 // Program is not in function
DynamicString *functionTypes; // Return type and param types of currently parsed function
Code progCode;                // Main body program code
Code functionsCode;           // Function def. program code
Stack *notDefinedCalls;       // Stack of not defined function calls
Token token, prevToken;       // Current and previous token

const int precTable[8][8] = {  // on top of the stack is:
    {R, L, L, R, L, R, R, R},  // +
    {R, R, L, R, L, R, R, R},  // *
    {L, L, L, E, L, L, L, N},  // (
    {R, R, N, R, N, R, R, R},  // )
    {R, R, N, R, N, R, R, R},  // id
    {L, L, L, R, L, N, R, R},  // comparison1 (< > <= >=)
    {L, L, L, R, L, L, N, R},  // comparison2 (=== !==)
    {L, L, L, N, L, L, L, N}}; // $
  // +  *  (  )  id c1 c2 $  <- incoming token

#define numOfExprRules 18
// rules are in reversed order because of stack
const TokenType exprRules[numOfExprRules][3] = {
    {TYPE_ID, TYPE_UNDEF, TYPE_UNDEF},         // E <- ID
    {TYPE_INT, TYPE_UNDEF, TYPE_UNDEF},        // E <- INT
    {TYPE_FLOAT, TYPE_UNDEF, TYPE_UNDEF},      // E <- FLOAT
    {TYPE_STRING, TYPE_UNDEF, TYPE_UNDEF},     // E <- STRING
    {TYPE_FUNID, TYPE_UNDEF, TYPE_UNDEF},      // E <- FUNID
    {TYPE_NULL, TYPE_UNDEF, TYPE_UNDEF},       // E <- NULL
    {TYPE_EXPR, TYPE_ADD, TYPE_EXPR},          // E <- E + E
    {TYPE_EXPR, TYPE_SUB, TYPE_EXPR},          // E <- E - E
    {TYPE_EXPR, TYPE_MUL, TYPE_EXPR},          // E <- E * E
    {TYPE_EXPR, TYPE_DIV, TYPE_EXPR},          // E <- E / E
    {TYPE_EXPR, TYPE_CONCAT, TYPE_EXPR},       // E <- E . E
    {TYPE_RBRACKET, TYPE_EXPR, TYPE_LBRACKET}, // E <- (E)
    {TYPE_EXPR, TYPE_EQTYPES, TYPE_EXPR},      // E <- E === E
    {TYPE_EXPR, TYPE_NOTEQTYPES, TYPE_EXPR},   // E <- E !== E
    {TYPE_EXPR, TYPE_LESS, TYPE_EXPR},         // E <- E < E
    {TYPE_EXPR, TYPE_GREATER, TYPE_EXPR},      // E <- E > E
    {TYPE_EXPR, TYPE_LESSEQ, TYPE_EXPR},       // E <- E <= E
    {TYPE_EXPR, TYPE_GREATEREQ, TYPE_EXPR}     // E <- E >= E
};

/**
 * @brief Returns new token from scanner
 *
 * @param includingComms 1 - comments should be included
 * @return Token - new token
 */
Token newToken(int includingComms)
{
    token = getToken();

    if (token.type == TYPE_LEXERR)
    {
        makeError(ERR_LEX);
    }

    // Skipping comments
    if (!includingComms && token.type == TYPE_COMM)
        token = newToken(includingComms);
    return token;
}

/**
 * @brief error function
 *
 * @param err
 */
void makeError(ErrorType err)
{
    // Store first error
    if (firstError == 0)
    {
        firstError = err;
    }

    // Skipping section of code with error in it
    while (token.type != TYPE_RBRACES && token.type != TYPE_LBRACES && token.type != TYPE_SEMICOLON)
    {
        if (token.type == TYPE_EOF)
            return;
        token = newToken(0);
    }
}

/**
 * @brief Get the right symtable
 *
 * @param global 1 - global, 0 - local
 * @return Symtable*
 */
Symtable *getTable(int global)
{
    if (global)
        return globalST;
    return localST;
}

/**
 * @brief Get the right code string to generate to
 *
 * @param global 1 - global, 0 - local
 * @return DynamicString*
 */
DynamicString *getCode(int global)
{
    if (global)
        return progCode.string;
    return functionsCode.string;
}

/**
 * @brief Get the poiter to the right code string to generate to
 * 
 * @param global 1 - global, 0 - local
 * @return DynamicString** 
 */
DynamicString **getCodePtr(int global)
{
    if (global)
        return &(progCode.string);
    return &(functionsCode.string);
}

/**
 * @brief Get the pointer to the right code struct to generate to 
 * 
 * @param global 1 - global, 0 - local
 * @return Code* 
 */
Code *getCodeStruct(int global)
{
    if (global)
        return &progCode;
    return &functionsCode;
}

/**
 * @brief Fill ST with builtin functions information
 *
 * @param stable Table to fill
 */
void builtInFuncFillST(Symtable *stable)
{
    STItemData data;
    data.funData.funTypes = (char *)malloc(sizeof(char) * 5);
    data.funData.defined = 1;

    strcpy(data.funData.funTypes, "vU");
    ST_insertItem(stable, "write", ST_ITEM_TYPE_FUNCTION, data);

    strcpy(data.funData.funTypes, "i");
    ST_insertItem(stable, "readi", ST_ITEM_TYPE_FUNCTION, data);
    strcpy(data.funData.funTypes, "f");
    ST_insertItem(stable, "readf", ST_ITEM_TYPE_FUNCTION, data);
    strcpy(data.funData.funTypes, "s");
    ST_insertItem(stable, "reads", ST_ITEM_TYPE_FUNCTION, data);

    strcpy(data.funData.funTypes, "fx");
    ST_insertItem(stable, "floatval", ST_ITEM_TYPE_FUNCTION, data);
    strcpy(data.funData.funTypes, "ix");
    ST_insertItem(stable, "intval", ST_ITEM_TYPE_FUNCTION, data);
    strcpy(data.funData.funTypes, "Sx");
    ST_insertItem(stable, "strval", ST_ITEM_TYPE_FUNCTION, data);

    strcpy(data.funData.funTypes, "is");
    ST_insertItem(stable, "strlen", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "ord", ST_ITEM_TYPE_FUNCTION, data);

    strcpy(data.funData.funTypes, "si");
    ST_insertItem(stable, "chr", ST_ITEM_TYPE_FUNCTION, data);

    strcpy(data.funData.funTypes, "Ssii");
    ST_insertItem(stable, "substring", ST_ITEM_TYPE_FUNCTION, data);

    free(data.funData.funTypes);
}

/**
 * @brief Converts keyword type to variable type (tokne type)
 *
 * @param keyword Keyword of the type to convert
 * (KEYWORD_INT, KEYWORD_FLOAT, KEYWORD_STRING, KEYWORD_VOID)
 * @return Token type for constant of given keyword type
 * (for other keywords returns TYPE_UNDEF)
 */
TokenType keywordType2VarType(KeyWord keyword)
{
    switch (keyword)
    {
    case KEYWORD_INT:
        return TYPE_INT;
        break;
    case KEYWORD_FLOAT:
        return TYPE_FLOAT;
        break;
    case KEYWORD_STRING:
        return TYPE_STRING;
        break;
    case KEYWORD_VOID:
        return TYPE_VOID;
        break;
    default:
        return TYPE_UNDEF;
        break;
    }
    return TYPE_UNDEF;
}

/**
 * @brief Get the char representation of TokenType ('i','f','s','V')
 *
 * @param type Type of token
 * @return ascii value of char
 */
int getTypeChar(TokenType type)
{
    if (type == TYPE_INT)
        return 'i';
    else if (type == TYPE_FLOAT)
        return 'f';
    else if (type == TYPE_STRING)
        return 's';
    else if (type == TYPE_VOID)
        return 'V';
    return 0;
}

// ---------RULES----------

/**
 * @brief Function for prog rule
 *
 * @return ErrorType
 */
ErrorType ruleProg()
{
    ErrorType err = 0;
    token = newToken(1);
    
    // PROLOG
    if (token.type != TYPE_BEGIN)
    {
        fprintf(stderr, "Expected <?php at beggining of the file!\n");
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    if (token.type != TYPE_SEMICOLON)
    {
        fprintf(stderr, "Expected \";\" after declare(strict_types=1) on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    // <st-list>
    err = ruleStatList(false);

    return err;
}

/**
 * @brief Function for statement list rule
 *
 * @return ErrorType
 */
ErrorType ruleStatList(bool isInBlock)
{
    ErrorType err = 0;
    ErrorType tmpErr = 0;
    int openBracesBlocks = isInBlock ? 1 : 0;

    while (1)
    {
        // Count open braces
        if (token.type == TYPE_LBRACES)
        {
            openBracesBlocks++;
            token = newToken(0);
        }

        // <return>
        while (token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_RETURN)
        {
            tmpErr = ruleReturn();
            if (!err)
                err = tmpErr;
        }

        // EPILOG
        if (token.type == TYPE_EOF)
            return err;

        if (token.type == TYPE_END)
        {
            token = newToken(0);
            if (token.type == TYPE_EOF)
            {
                return err;
            }
            fprintf(stderr, "Expected EOF after \"?>\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }

        // Check end of {<stat-list>}
        if (token.type == TYPE_RBRACES)
        {
            openBracesBlocks--;
            if (openBracesBlocks < 0)
            {
                fprintf(stderr, "Unexpected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return (ERR_SYN);
            }
            if (openBracesBlocks == 0)
                break;
            token = newToken(0);
        }

        // Statement rule
        tmpErr = ruleStat();
        if (!err)
            err = tmpErr;
    }

    return err;
}

/**
 * @brief Function for statement rule
 *
 * @return ErrorType
 */
ErrorType ruleStat()
{
    ErrorType err = 0;
    ErrorType errTmp = 0;
    int isEmpty;

    if (token.type == TYPE_KEYWORD)
    {
        switch (token.attribute.keyword)
        {
        case KEYWORD_IF:
            token = newToken(0);

            // (
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // (<expr>)
            err = exprAnal(&isEmpty, 0);

            // )
            if (token.type != TYPE_RBRACKET)
            {
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);

            // Check if condition is empty
            if (isEmpty)
            {
                fprintf(stderr, "Expected expression in if statement on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }

            static int ifCount = 0;
            ifCount++;
            int curIfCount = ifCount; // Fixes 'if' within another 'if'
            bool outerIf = false;     // Whether current 'if' is not contained in another 'if' or 'while'

            // Set last unconditioned line
            if (getCodeStruct(isGlobal)->lastUnconditionedLine == -1)
            {
                outerIf = true;
                getCodeStruct(isGlobal)->lastUnconditionedLine = getCode(isGlobal)->numOfChars;
            }

            // Generate code for if start
            CODEifStart(getCodePtr(isGlobal), curIfCount, getCodeStruct(isGlobal)->lastUnconditionedLine);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList(true);
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // ELSE
            errTmp = 0;
            if (token.type != TYPE_KEYWORD)
            {
                errTmp = 2;
            }
            else if (token.attribute.keyword != KEYWORD_ELSE)
            {
                errTmp = 2;
            }

            if (errTmp)
            {
                fprintf(stderr, "Expected ELSE after \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // Generate code for else
            CODEelse(getCode(isGlobal), curIfCount);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList(true);
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // Unset last unconditioned line, if current 'if' ends conditioned code block
            if (outerIf)
                getCodeStruct(isGlobal)->lastUnconditionedLine = -1;

            // Generate code for if end
            CODEifEnd(getCode(isGlobal), curIfCount);

            break;

        case KEYWORD_WHILE:
            token = newToken(0);

            // (
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            static int whileCount = 0;
            whileCount++;
            int curWhileCount = whileCount; // Fixes 'while' within another 'while'
            bool outerWhile = false;        // Whether current 'if' is not contained in another 'if' or 'while'

            // Set last unconditioned line
            if (getCodeStruct(isGlobal)->lastUnconditionedLine == -1)
            {
                outerWhile = true;
                getCodeStruct(isGlobal)->lastUnconditionedLine = getCode(isGlobal)->numOfChars;
            }

            // Generate code for while start
            CODEwhileStart(getCodePtr(isGlobal), curWhileCount, getCodeStruct(isGlobal)->lastUnconditionedLine);

            // (<expr>)
            err = exprAnal(&isEmpty, 0);

            // )
            if (token.type != TYPE_RBRACKET)
            {
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);

            // Check if condition is empty
            if (isEmpty)
            {
                fprintf(stderr, "Expected expression in while statement on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }

            // Generate code for while condition
            CODEwhileCond(getCode(isGlobal), curWhileCount);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList(true);
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // Unset last unconditioned line, if current 'while' ends conditioned code block
            if (outerWhile)
                getCodeStruct(isGlobal)->lastUnconditionedLine = -1;

            // Generate code for while end
            CODEwhileEnd(getCode(isGlobal), curWhileCount);

            break;

        case KEYWORD_FUNCTION:
            // Defining inside function
            if (!isGlobal)
            {
                fprintf(stderr, "Definition of function inside function on line %d!\n", token.rowNumber);
                makeError(ERR_FUNDEF);
                while (token.type != TYPE_RBRACES)
                {
                    token = newToken(0);
                }
                return ERR_FUNDEF;
                break;
            }
            token = newToken(0);

            // FUNID
            if (token.type != TYPE_FUNID)
            {
                fprintf(stderr, "Expected function name after \"function\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }

            // Check redefinition
            STItem *foundFunction = ST_searchTable(getTable(1), token.attribute.dString->string);
            if (foundFunction != NULL){
                if (foundFunction->type == ST_ITEM_TYPE_FUNCTION && foundFunction->data.funData.defined == true)
                {
                    fprintf(stderr, "Redefinition of function \"%s\" on line %d!\n", token.attribute.dString->string, token.rowNumber);
                    makeError(ERR_FUNDEF);
                    return ERR_FUNDEF;
                    break;
                }
            }
            DynamicString *funId;
            funId = DS_init();
            DS_appendString(funId, token.attribute.dString->string);
            token = newToken(0);

            // Generate code for function start
            CODEfuncDef(getCode(0), funId->string);

            // (
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <params>
            ST_freeTable(localST);
            localST = ST_initTable(8);
            DS_deleteAll(functionTypes);
            DS_append(functionTypes, 'V'); // Set unspecified return type
            err = ruleParams();

            // )
            if (token.type != TYPE_RBRACKET)
            {
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // Inserting incomplete function into symtable (needed for recursion)
            STItemData newFunData;
            newFunData.funData.defined = 1;
            newFunData.funData.funTypes = functionTypes->string;
            ST_insertItem(getTable(1), funId->string, ST_ITEM_TYPE_FUNCTION, newFunData);

            // <funcdef>
            errTmp = ruleFuncdef();
            if (err == 0)
            {
                err = errTmp;
            }

            // Inserting function into symtable
            newFunData.funData.funTypes = functionTypes->string;
            ST_insertItem(getTable(1), funId->string, ST_ITEM_TYPE_FUNCTION, newFunData);

            // Generate code for function end
            if (isLower(functionTypes->string[0]))
            {
                CODEfuncDefEnd(getCode(false), funId->string, false);
            }
            else
            {
                CODEfuncDefEnd(getCode(false), funId->string, true);
            }

            DS_dispose(funId);
            DS_dispose(functionTypes); // ST_insertItem() copies funTypes string -> Original can be freed
            functionTypes = DS_init();
            DS_append(functionTypes, 'V'); // Set unspecified return type
            break;

        default:
            fprintf(stderr, "Unexpected keyword on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
            break;
        }
    }
    else // Not keyword
    {
        err = ruleAssign();
    }
    return err;
}


/**
 * @brief Function for definition rule
 *
 * @return ErrorType
 */
ErrorType ruleFuncdef()
{
    ErrorType err = 0;
    ErrorType tmpErr = 0;
    bool canBeNull = false;
    isGlobal = 0; // Set parsing in function

    // : ?TYPE
    if (token.type == TYPE_COLON)
    {
        token = newToken(0);

        // ?
        if (token.type == TYPE_QMARK)
        {
            canBeNull = true;
            token = newToken(0);
        }

        // TYPE
        if (token.type != TYPE_KEYWORD)
        {
            tmpErr = 1;
        }
        else if (!isKeywordType(token.attribute.keyword))
        {
            tmpErr = 1;
        }

        if (tmpErr)
        {
            fprintf(stderr, "Expected return type after \":\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
        }

        functionTypes->string[0] = getTypeChar(keywordType2VarType(token.attribute.keyword)); // Set specified return type
        if (canBeNull && functionTypes->string[0] != 'V')
            functionTypes->string[0] -= 32; // Capitalize if function can return null or other type
        token = newToken(0);
    }

    // {
    if (token.type != TYPE_LBRACES)
    {
        fprintf(stderr, "Expected \"{\" or \":\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return ERR_SYN;
    }
    token = newToken(0);

    // <stat_list>
    err = ruleStatList(true);

    // }
    if (token.type != TYPE_RBRACES)
    {
        fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return ERR_SYN;
    }
    token = newToken(0);

    isGlobal = 1; // Set parsing in global scale

    return err;
}

/**
 * @brief Function for assign rule
 *
 * @return ErrorType
 */
ErrorType ruleAssign()
{
    ErrorType err = 0;
    int isEmpty;

    // <assign> => <expr> ;
    if (token.type != TYPE_ID)
    {
        err = exprAnal(&isEmpty, 0);
        // Pops unused expression result
        DS_appendString(getCode(isGlobal),"POPS GF@void\n");
        if (isEmpty) {
            fprintf(stderr, "Expected expression on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
        }

        // ;
        if (token.type != TYPE_SEMICOLON)
        {
            fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
        }
        token = newToken(0);
        return err;
    }
    else // <assign> => ID <expr> ;
    {
        STItem *item = ST_searchTable(getTable(isGlobal), DS_string(token.attribute.dString));
        prevToken = token;
        token = newToken(0);

        if (token.type == TYPE_ASSIGN) // <assign> => ID = <expr> ;
        {
            token = newToken(0);
            err = exprAnal(&isEmpty, 0);
            if (isEmpty) {
                fprintf(stderr, "Expected expression in assignment on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
            }

            // Variable not defined
            if (item == NULL)
            {
                STItemData STdata;
                STdata.varData.VarType = isEmpty;
                ST_insertItem(getTable(isGlobal), DS_string(prevToken.attribute.dString), ST_ITEM_TYPE_VARIABLE, STdata);
                CODEdefVar(getCodePtr(isGlobal), DS_string(prevToken.attribute.dString), getCodeStruct(isGlobal)->lastUnconditionedLine);
            }
            CODEassign(getCode(isGlobal), prevToken);

            // ;
            if (token.type != TYPE_SEMICOLON)
            {
                fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);
        }
        else
        {
            err = exprAnal(&isEmpty, 1);
            // Pops unused expression result
            DS_appendString(getCode(isGlobal),"POPS GF@void\n");
            if (token.type != TYPE_SEMICOLON)
            {
                fprintf(stderr, "Expected \";\" on line %d!\n", prevToken.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);
        }
    }

    return err;
}

/**
 * @brief Function for params rule
 *
 * @return ErrorType
 */
ErrorType ruleParams()
{
    // Stack of param IDs for POPping param value in code
    Stack *paramStack = STACK_init();
    STACK_popAll(paramStack);

    // epsilon
    // Check empty parametr list
    if (token.type == TYPE_RBRACKET)
        return 0;

    ErrorType err = 0;
    ErrorType tmpErr = 0;

    // <param>
    err = ruleParam(paramStack);

    // <params_2>
    tmpErr = ruleParams2(paramStack);
    if (!err)
        err = tmpErr;

    // Generate code for POPping param value in code
    int paramTypeIndex = functionTypes->numOfChars-1; //Index of last param type (popping form behind)
    while (!STACK_isEmpty(paramStack))
    {
        CODEpopParam(getCode(false), STACK_top(paramStack)->attribute.dString->string, functionTypes->string[paramTypeIndex]);
        STACK_pop(paramStack);
        paramTypeIndex--;
    }
    STACK_dispose(paramStack);

    return err;
}

/**
 * @brief Function for params2 rule
 *
 * @return ErrorType
 */
ErrorType ruleParams2(Stack *paramStack)
{
    // epsilon
    // Check end of parametr list
    if (token.type == TYPE_RBRACKET)
        return 0;

    ErrorType err = 0;
    ErrorType tmpErr = 0;

    // ,
    if (token.type != TYPE_COMMA)
    {
        fprintf(stderr, "Expected \",\" between parametrs of function on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    // <param>
    err = ruleParam(paramStack);

    // <params_2>
    tmpErr = ruleParams2(paramStack);
    if (!err)
        err = tmpErr;

    return err;
}

/**
 * @brief Function for param rule
 *
 * @return ErrorType
 */
ErrorType ruleParam(Stack *paramStack)
{
    ErrorType err = 0;
    bool canBeNull = 0;
    char paramType = '!';

    if (token.type != TYPE_QMARK && token.type != TYPE_KEYWORD)
    {
        fprintf(stderr, "Expected type of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    // ?
    if (token.type == TYPE_QMARK)
    {
        canBeNull = true;
        token = newToken(0);
    }

    // TYPE
    if (token.type != TYPE_KEYWORD)
    {
        err = 1;
    }
    else if (!isKeywordType(token.attribute.keyword))
    {
        err = 1;
    }
    if (err)
    {
        fprintf(stderr, "Expected type of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    paramType = getTypeChar(keywordType2VarType(token.attribute.keyword));
    if (canBeNull)
        paramType -= 32; // Capitalize if param can be null
    token = newToken(0);

    // ID
    if (token.type != TYPE_ID)
    {
        fprintf(stderr, "Expected a name of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    // Check same name params
    if (ST_searchTable(getTable(0), token.attribute.dString->string) != NULL)
    {
        fprintf(stderr, "Redefinition of function parametr \"%s\" on line %d!\n", token.attribute.dString->string, token.rowNumber);
        makeError(ERR_OTHER);
        return (ERR_OTHER);
    }

    // Generate code for parametr
    CODEparam(getCode(false), token.attribute.dString->string);
    STACK_push(paramStack, token); // Pushing param name on stack for later value pop in ruleParams()

    // Insert param as local variable into symtable
    STItemData newVarData;
    newVarData.varData.VarType = paramType;
    ST_insertItem(getTable(0), token.attribute.dString->string, ST_ITEM_TYPE_VARIABLE, newVarData);

    // Adding param to functionTypes
    DS_append(functionTypes, paramType);
    token = newToken(0);

    return err;
}

/**
 * @brief Function for return rule
 *
 * @return ErrorType
 */
ErrorType ruleReturn()
{
    ErrorType err = 0;
    char varType = 'i';
    char *code = NULL;

    // RETURN
    if (token.type != TYPE_KEYWORD)
        return 0; // epsilon
    if (token.attribute.keyword != KEYWORD_RETURN)
        return 0; // epsilon
    token = newToken(0);

    // <expr> ;
    if (token.type != TYPE_SEMICOLON)
    {
        err = exprAnal(&varType, 0);

        // ;
        if (token.type != TYPE_SEMICOLON)
        {
            fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }

        // Returns a value but is of type void
        if (functionTypes->string[0] == 'V' && isGlobal == 0 && err == 0)
        {
            fprintf(stderr, "Function is of type void, but returns a value on line %d!\n", token.rowNumber);
            makeError(ERR_EXPRES);
            return (ERR_EXPRES);
        }

        // Generate code for returning a value
        if (isGlobal)
        {
            code = "EXIT int@0\n";
            DS_appendString(getCode(true), code);
        }
        else
        {
            CODEfuncReturn(getCode(false), functionTypes->string[0], token.rowNumber);
        }

        token = newToken(0);
        return err;
    } // ;
    else if (isLower(functionTypes->string[0]) && isGlobal == 0)
    { // Must be ";" -> checking return value
        fprintf(stderr, "No return value in non-void function on line %d!\n", token.rowNumber);
        makeError(ERR_EXPRES);
        return (ERR_EXPRES);
    }
    token = newToken(0);

    // Generate code for return without a value
    if (isGlobal)
    {
        code = "EXIT int@0\n";
        DS_appendString(getCode(true), code);
    }
    else
    {
        code = "PUSHS nil@nil\nPOPFRAME\nRETURN\n";
        DS_appendString(getCode(false), code);
    }

    return err;
}

/**
 * @brief Get the precedecne table index
 *
 * @param token Inspected token
 * @return int index of token type in precedence table
 */
int getPrecTableIndex(Token token)
{
    switch (token.type)
    {
    case TYPE_ADD:
    case TYPE_SUB:
    case TYPE_CONCAT:
        return 0;

    case TYPE_MUL:
    case TYPE_DIV:
        return 1;

    case TYPE_LBRACKET:
        return 2;

    case TYPE_RBRACKET:
        return 3;

    case TYPE_ID:
    case TYPE_FUNID:
    case TYPE_INT:
    case TYPE_FLOAT:
    case TYPE_STRING:
    case TYPE_NULL:
        return 4;

    case TYPE_LESS:
    case TYPE_GREATER:
    case TYPE_LESSEQ:
    case TYPE_GREATEREQ:
        return 5;

    case TYPE_EQTYPES:
    case TYPE_NOTEQTYPES:
        return 6;

    case TYPE_STACKEMPTY:
        return 7;

    default:
        break;
    }

    return -1;
}

/**
 * @brief Finds reduction of expression
 *
 * @param tokenArr Array containing operand, operator, operand
 * @return int used rule
 */
int exprUseRule(Token *tokenArr)
{
    // Cycle through all rules
    for (int rule = 0; rule < numOfExprRules; rule++)
    {
        int match = 1;

        // Compare rule with tokenArr
        for (int i = 0; i < 3; i++)
        {
            // Rule is not matching
            if (tokenArr[i].type != exprRules[rule][i])
            {
                match *= 0;
                break;
            }
        }
        if (match)
        {
            return rule;
        }
    }
    return -1;
}

/**
 * @brief Function for function call
 *
 * @return ErrorType
 */
ErrorType functionCallCheckAndProcess()
{
    int argCount = 0;
    Token funID = token;
    int isEmpty = 0;

    STItem *item = ST_searchTable(getTable(1), DS_string(funID.attribute.dString));
    // Function not defined
    if (item == NULL) 
    { 
        if (isGlobal)
        {
            fprintf(stderr, "Function \"%s\" on line %d is not defined!\n", DS_string(funID.attribute.dString), funID.rowNumber);
            makeError(ERR_UNDEF);
            return ERR_UNDEF;
        }
        else // Store undefined function to symtable
        {
            STItemData newFuncData;
            newFuncData.funData.defined = 0;
            newFuncData.funData.funTypes = "";
            ST_insertItem(getTable(1), DS_string(funID.attribute.dString), ST_ITEM_TYPE_FUNCTION, newFuncData);
            item = ST_searchTable(getTable(1), DS_string(funID.attribute.dString));
        }
    }

    // Store function call to stack for checking later
    if (item->data.funData.defined == 0)
    {
        STACK_push(notDefinedCalls, funID);
    }

    int paramCount = -1;
    // If function is defined, get number of parameters
    if (item->data.funData.defined == 1 && item->data.funData.funTypes[1] != 'U')
        paramCount = strlen(item->data.funData.funTypes) - 1; //-1 <- first char is return type
    token = newToken(0);

    // (
    if (token.type != TYPE_LBRACKET)
    {
        fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    // )
    while (token.type != TYPE_RBRACKET)
    {
        if (isValueType(token.type) || isBracket(token.type))
        {
            argCount++;
            if (argCount > paramCount && paramCount != -1 && item->data.funData.defined == 1)
            {
                fprintf(stderr, "Too many arguments in function call on line %d!\n", token.rowNumber);
                makeError(ERR_RUNPAR);
                return ERR_RUNPAR;
            }
        }
        exprAnal(&isEmpty, 0);

        // Empty argument
        if (isEmpty)
        {
            fprintf(stderr, "Empty argument in function call on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
        }

        // ,
        if (token.type == TYPE_COMMA)
        {
            token = newToken(0);
            // after , is )
            if (token.type == TYPE_RBRACKET)
            {
                fprintf(stderr, "Empty argument in function call on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return (ERR_SYN);
            }
        }
        else if (token.type != TYPE_RBRACKET) // after value is not ) or ,
        {
            fprintf(stderr, "Expected \",\" or \")\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }
    }

    // Too many arguments
    if (argCount < paramCount && paramCount != -1 && item->data.funData.defined == 1)
    {
        fprintf(stderr, "Too few arguments in function call on line %d!\n", token.rowNumber);
        makeError(ERR_RUNPAR);
        return ERR_RUNPAR;
    }
    else if (item->data.funData.defined == 0) // Function not defined -> store number of arguments
    {
        Token tmpToken;
        tmpToken.type = TYPE_INT;
        tmpToken.attribute.intV = argCount;
        STACK_push(notDefinedCalls, tmpToken);
    }

    token = funID; // Set token back to function ID
    CODEfuncCall(getCode(isGlobal), funID, argCount);
    return 0;
}

/**
 * @brief Semantics rules for expression
 *
 * @param ruleUsed Index of used rule
 * @param tokenArr Array of used tokens while reducing (operand, operator, operand)
 * @param endToken Last legit token
 * @return ErrorType
 */
ErrorType rulesSematics(int ruleUsed, Token *tokenArr, Token endToken)
{
    // Check if variable is initialized
    if (ruleUsed == 0)
    {
        STItem *item = ST_searchTable(getTable(isGlobal), DS_string(tokenArr[0].attribute.dString));
        if (item == NULL)
        {
            token = endToken;
            fprintf(stderr, "Usage of not initialized variable \"%s\" on line %d!\n", DS_string(tokenArr[0].attribute.dString), token.rowNumber);
            makeError(ERR_UNDEF);
            return ERR_UNDEF;
        }
    }

    // Generate code for expression
    CODEarithmetic(getCode(isGlobal), ruleUsed, tokenArr);
    return 0;
}



/**
 * @brief Expression analysis
 *
 * @param isEmpty Pointer for int, stores 1 if expression is empty
 * @param usePrevToken use - 1, dont use - 0
 * @return ErrorType
 */
ErrorType exprAnal(int *isEmpty, int usePrevToken)
{
    Token tmpToken;
    Token endToken = token; // last terminal token
    ErrorType err = 0;
    int done = 0; // 1 if whole expression is loaded
    *isEmpty = 0;

    // Init expression stack
    Stack *stack = STACK_init();
    tmpToken.type = TYPE_STACKEMPTY;
    STACK_push(stack, tmpToken);

    if (usePrevToken)
    {
        tmpToken.type = TYPE_LESSPREC;
        STACK_push(stack, tmpToken);
        STACK_push(stack, prevToken);

        // Nothing to proccess after prevToken
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type))
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }
    }
    else
    {
        // Empty expression
        if (!isOperatorType(token.type) && !isValueType(token.type) && token.type != TYPE_LBRACKET)
        {
            STACK_dispose(stack);
            *isEmpty = 1;
            return err;
        }
    }
    
    // Function call
    if (token.type == TYPE_FUNID)
    {
        err = functionCallCheckAndProcess();
    }

    while (1)
    {
        // Init token array for reduction
        Token undefToken;
        undefToken.type = TYPE_UNDEF;
        Token tokenArrExpr[3] = {undefToken, undefToken, undefToken};

        int stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
        int tokenPrecIndex = getPrecTableIndex(token);

        // Expression is on top of stack -> load precedence of terminal under expression
        if (STACK_top(stack)->type == TYPE_EXPR)
        {
            STACK_pop(stack);
            stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }

        // Precedence of token not found
        if (stackPrecIndex == -1 || tokenPrecIndex == -1)
        {
            fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
            STACK_dispose(stack);
            token = endToken;
            makeError(ERR_SYN);
            return ERR_SYN;
        }

        switch (precTable[stackPrecIndex][tokenPrecIndex])
        {
        case E: // =
            STACK_push(stack, token);
            token = newToken(0);
            break;

        case L: // <
            // If expression is on top, push < behind it
            if (isOperatorType(token.type) && STACK_top(stack)->type == TYPE_EXPR)
            {
                STACK_pop(stack);
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
                tmpToken.type = TYPE_EXPR;
                STACK_push(stack, tmpToken);
            }
            else // Expression is not on top, push <
            {
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
            }

            // Push new token
            STACK_push(stack, token);
            token = newToken(0);
            break;

        case R: // >
            // Reduct beteween < and >
            for (int i = 0; STACK_top(stack)->type != TYPE_LESSPREC; i++)
            {
                tokenArrExpr[i] = *STACK_top(stack);
                STACK_pop(stack);

                // Array overflow
                if (i > 2)
                {
                    token = endToken;
                    fprintf(stderr, "Wrong number of operands on line %d!\n", token.rowNumber);
                    STACK_dispose(stack);
                    makeError(ERR_SYN);
                    return ERR_SYN;
                }
            }

            // Check expression rules
            int usedRule = exprUseRule(tokenArrExpr);
            if (usedRule == -1)
            {
                token = endToken;
                fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
                STACK_dispose(stack);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            err = rulesSematics(usedRule, tokenArrExpr, endToken);

            STACK_pop(stack); // pop <

            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
            break;

        case N: // error
            STACK_dispose(stack);
            // Returning right bracket for conditions and loops
            if (token.type == TYPE_RBRACKET)
            {
                return 0;
            }
            token = endToken;
            fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
        }

        // Function call
        if (token.type == TYPE_FUNID)
        {
            err = functionCallCheckAndProcess();
        }

        // Non-expression token loaded
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type) && !done)
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }

        // Expression is on top of the stack
        if (done && STACK_top(stack)->type == TYPE_EXPR)
        {
            STACK_pop(stack);
            // Only TYPE_EXPR is on stack -> expression is correctly parsed
            if (STACK_top(stack)->type == TYPE_STACKEMPTY)
            {
                token = endToken;
                break;
            }
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }
    }

    STACK_dispose(stack);
    return err;
}

/**
 * @brief Check if function is defined and if counts of arguments and paramaters are same
 * 
 * @param notDefinedCalls Stack with function calls and number of its arguments
 * @return ErrorType 
 */
ErrorType checkIfDefined(Stack *notDefinedCalls)
{
    ErrorType err = 0;
    int argCount = 0;
    Token *bottom = STACK_bottom(notDefinedCalls);
    Token funID;
    while (bottom != NULL)
    {
        funID = *bottom;
        STItem *item = ST_searchTable(getTable(1), DS_string(funID.attribute.dString));

        // Check if function is defined
        if (item->data.funData.defined == 0)
        {
            fprintf(stderr, "Function \"%s\" on line %d is not defined!\n", DS_string(funID.attribute.dString), funID.rowNumber);
            makeError(ERR_FUNDEF);
            return ERR_FUNDEF;
        }

        // Get counts of arguments and paramaters
        argCount = strlen(item->data.funData.funTypes) - 1;
        STACK_popBottom(notDefinedCalls);
        // Check if counts of arguments and paramaters are same
        if (argCount != STACK_bottom(notDefinedCalls)->attribute.intV)
        {
            fprintf(stderr, "Wrong count of arguments in function call on line %d!\n", funID.rowNumber);
            makeError(ERR_RUNPAR);
            return ERR_RUNPAR;
        }
        STACK_popBottom(notDefinedCalls);
        bottom = STACK_bottom(notDefinedCalls);
    }
    return err;
}

/**
 * @brief Main parser function
 *
 * @return int error code
 */
int parser()
{
    // Init global variables
    firstError = 0;
    globalST = ST_initTable(16);
    localST = NULL; //Inits in function definition
    isGlobal = 1;
    notDefinedCalls = STACK_init();
    functionTypes = DS_init();
    functionsCode.string = DS_init();
    functionsCode.lastUnconditionedLine = -1;
    progCode.string = DS_init();
    progCode.lastUnconditionedLine = -1;

    // Stores built-in functions info to global symbol table
    builtInFuncFillST(globalST);

    // Generate code for built-in functions and label _main
    CODEbuiltInFunc(getCode(false));
    CODEmain(getCode(true));

    //  <prog> => BEGIN DECLARE_ST <stat_list>
    ruleProg();

    // Check if all functions called from other functions are defined
    checkIfDefined(notDefinedCalls);

    // Generate code for main exit 0
    DS_appendString(getCode(true), "\nEXIT int@0\n");

    // Print code if no error occured
    if (firstError == 0)
    {
        printf("%s", DS_string(getCode(false)));
        printf("%s", DS_string(getCode(true)));
    }

    DS_dispose(functionsCode.string);
    DS_dispose(progCode.string);
    DS_dispose(functionTypes);
    ST_freeTable(globalST);
    return firstError;
}