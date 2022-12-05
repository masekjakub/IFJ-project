/**
 * @file parser.c
 * @authors Jakub Mašek, Martin Zelenák
 * @brief parser for IFJ22 translator
 * @version 0.1
 * @date 16-10-2022
 *
 * @copyright Copyright (c) 2022
 *
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

int firstError;               // first encountered error
Symtable *globalST;           // global symtable
Symtable *localST;            // local symtable
int isGlobal;                 // program is not in function
DynamicString *functionTypes; // Return type and param types of currently parsed function
Code progCode;                // Main body program code
Code functionsCode;           // Function def. program code
Stack *notDefinedCalls;       // Stack of not defined function calls

const int precTable[8][8] = {
    {R, L, L, R, L, L, L, R},  // +
    {R, R, L, R, L, L, L, R},  // *
    {L, L, L, E, L, L, L, N},  // (
    {R, R, N, R, N, R, R, R},  // )
    {R, R, N, R, N, R, R, R},  // id
    {L, L, L, R, L, N, R, R},  // comparison1 (< > <= >=)
    {L, L, L, R, L, L, N, R},  // comparison2 (=== !==)
    {L, L, L, N, L, L, L, N}}; // $
                               // +  *  (  )  id c1 c2 $

Token token, prevToken;

Token *tokenArr; // simulation DELETE ME!

#define numOfExprRules 18
// rules are flipped because of stack
const TokenType exprRules[numOfExprRules][3] = {
    {TYPE_ID, TYPE_UNDEF, TYPE_UNDEF},         // E => ID
    {TYPE_INT, TYPE_UNDEF, TYPE_UNDEF},        // E => INT
    {TYPE_FLOAT, TYPE_UNDEF, TYPE_UNDEF},      // E => FLOAT
    {TYPE_STRING, TYPE_UNDEF, TYPE_UNDEF},     // E => STRING
    {TYPE_FUNID, TYPE_UNDEF, TYPE_UNDEF},      // E => FUNID
    {TYPE_NULL, TYPE_UNDEF, TYPE_UNDEF},       // E => NULL
    {TYPE_EXPR, TYPE_ADD, TYPE_EXPR},          // E => E + E
    {TYPE_EXPR, TYPE_SUB, TYPE_EXPR},          // E => E - E
    {TYPE_EXPR, TYPE_MUL, TYPE_EXPR},          // E => E * E
    {TYPE_EXPR, TYPE_DIV, TYPE_EXPR},          // E => E / E
    {TYPE_EXPR, TYPE_CONCAT, TYPE_EXPR},       // E => E . E
    {TYPE_RBRACKET, TYPE_EXPR, TYPE_LBRACKET}, // E => (E)
    {TYPE_EXPR, TYPE_EQTYPES, TYPE_EXPR},      // E => E === E
    {TYPE_EXPR, TYPE_NOTEQTYPES, TYPE_EXPR},   // E => E !== E
    {TYPE_EXPR, TYPE_LESS, TYPE_EXPR},         // E => E < E
    {TYPE_EXPR, TYPE_GREATER, TYPE_EXPR},      // E => E > E
    {TYPE_EXPR, TYPE_LESSEQ, TYPE_EXPR},       // E => E <= E
    {TYPE_EXPR, TYPE_GREATEREQ, TYPE_EXPR}};   // E => E >= E

/**
 * @brief Returns new token from scanner
 *
 * @param includingComms
 * @return Token
 */
Token newToken(int includingComms)
{
#ifdef scanner
    token = getToken();
#else
    token = getTokenSim(tokenArr); // odstranit tokenarr
#endif

    if (token.type == TYPE_LEXERR)
    {
        makeError(ERR_LEX);
    }

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
    if (firstError == 0)
    {
        firstError = err;
    }

    // Skipping section of code with error in it
    // while(token.type != TYPE_RBRACKET && token.type != TYPE_RBRACES && token.type != TYPE_SEMICOLON && token.type != TYPE_COMMA){
    while (token.type != TYPE_RBRACES && token.type != TYPE_LBRACES && token.type != TYPE_SEMICOLON)
    {
        if (token.type == TYPE_EOF)
            return;
        token = newToken(0);
    }
    // token = newToken(0);
    //  exit(err);
}

/**
 * @brief Get the Table object
 *
 * @param global
 * @return Symtable*
 */
Symtable *getTable(int global)
{
    if (global)
        return globalST;
    return localST;
}

DynamicString *getCode(int global)
{
    if (global)
        return progCode.string;
    return functionsCode.string;
}

DynamicString **getCodePtr(int global)
{
    if (global)
        return &(progCode.string);
    return &(functionsCode.string);
}

Code *getCodeStruct(int global)
{
    if (global)
        return &progCode;
    return &functionsCode;
}

/**
 * @brief fill ST with builtin functions info
 *
 * @param stable table to fill
 */
void builtInFuncFillST(Symtable *stable)
{
    STItemData data;
    data.funData.funTypes = (char *)malloc(sizeof(char) * 5);
    data.funData.defined = 1;

    strcpy(data.funData.funTypes, "vU"); // unlimited
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
    ;
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
 * @brief Get the char representation of TokenType ('i','f','s')
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
 * @brief Checks if prolog is right
 *
 * @param tokenArr
 * @return int error code
 */
ErrorType ruleProg() // remove tokenArr SIMULATION
{
    ErrorType err = 0;
    token = newToken(1);
    // prolog
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
 * @brief statement list rule
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
        // if (err)
        //     return err;

        if (token.type == TYPE_LBRACES)
        {
            openBracesBlocks++;
            token = newToken(0);
        }

        //<return>
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

        // statement rule
        tmpErr = ruleStat();
        if (!err)
            err = tmpErr;
    }

    return err;
}

/**
 * @brief statement rule
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
                // token = newToken(0);
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
            if (foundFunction != NULL)
                if (foundFunction->type == ST_ITEM_TYPE_FUNCTION && foundFunction->data.funData.defined == true)
                {
                    fprintf(stderr, "Redefinition of function \"%s\" on line %d!\n", token.attribute.dString->string, token.rowNumber);
                    makeError(ERR_FUNDEF);
                    return ERR_FUNDEF;
                    break;
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
    else // not keyword
    {
        err = ruleAssign();
    }
    return err;
}

/**
 * @brief identificator rule
 *
 * @return ErrorType
 */
/* udelat: kontrola inicializace
 */
ErrorType ruleId()
{
    ErrorType err = 0;

    return err;
}

/**
 * @brief function definition rule
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
 * @brief assign rule
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
        DS_appendString(getCode(isGlobal),"POPS GF@void\n");
        if (isEmpty) {
            fprintf(stderr, "Expected expression on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
        }
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
            STItemData data;
            token = newToken(0);
            err = exprAnal(&isEmpty, 0);
            if (isEmpty) {
                fprintf(stderr, "Expected expression in assignment on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
            }
            if (item == NULL)
            {
                STItemData STdata;
                STdata.varData.VarType = isEmpty;
                ST_insertItem(getTable(isGlobal), DS_string(prevToken.attribute.dString), ST_ITEM_TYPE_VARIABLE, data);
                CODEdefVar(getCodePtr(isGlobal), DS_string(prevToken.attribute.dString), getCodeStruct(isGlobal)->lastUnconditionedLine);
            }
            CODEassign(getCode(isGlobal), prevToken);

            if (token.type != TYPE_SEMICOLON)
            {
                fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);

            /*if (varType == 0)
            {
                //printf("EMPTY");
                return err;
            }*/
        }
        else
        {
            err = exprAnal(&isEmpty, 1);
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
 * @brief params rule
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
    while (!STACK_isEmpty(paramStack))
    {
        CODEpopValue(getCode(false), STACK_top(paramStack)->attribute.dString->string, false);
        STACK_pop(paramStack);
    }
    STACK_dispose(paramStack);

    return err;
}

/**
 * @brief params2 rule
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
 * @brief param rule
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
 * @brief return rule
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
        code = "POPFRAME\nRETURN\n";
        DS_appendString(getCode(false), code);
    }

    return err;
}

/**
 * @brief Get the Prec Table Index object
 *
 * @param token inspected token
 * @return int, index of token type in precedence table
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
 * @brief finds reduction of expression
 *
 * @param tokenArr array[3] of operands and operator
 * @return int used rule
 */
int exprUseRule(Token *tokenArr)
{

    /*for (int i = 0; i < 3; i++)
    {
        printf("%d ",tokenArr[i]);
    }
    printf("\n");
*/
    for (int rule = 0; rule < numOfExprRules; rule++)
    {
        int match = 1;
        for (int i = 0; i < 3; i++)
        {
            if (tokenArr[i].type != exprRules[rule][i]) // rule is not matching
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
 * @brief proccess function call
 *
 * @return ErrorType
 */
ErrorType functionCallCheckAndProcess()
{
    int argCount = 0;
    Token funID = token;
    int isEmpty = 0;

    STItem *item = ST_searchTable(getTable(1), DS_string(funID.attribute.dString));
    if (item == NULL)
    { // function not defined
        if (isGlobal)
        {
            fprintf(stderr, "Function \"%s\" on line %d is not defined!\n", DS_string(funID.attribute.dString), funID.rowNumber);
            makeError(ERR_UNDEF);
            return ERR_UNDEF;
        }
        else
        {
            STItemData newFuncData;
            newFuncData.funData.defined = 0;
            ST_insertItem(getTable(1), DS_string(funID.attribute.dString), ST_ITEM_TYPE_FUNCTION, newFuncData);
            item = ST_searchTable(getTable(1), DS_string(funID.attribute.dString));
        }
    }
    if (item->data.funData.defined == 0)
    {
        STACK_push(notDefinedCalls, funID);
    }

    int paramCount = -1;
    if (item->data.funData.defined == 1 && item->data.funData.funTypes[1] != 'U') // unlimited
        paramCount = strlen(item->data.funData.funTypes) - 1;                     //-1 <- first char is return type
    token = newToken(0);

    // (
    if (token.type != TYPE_LBRACKET)
    {
        fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    token = newToken(0);
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
        if (isEmpty)
        {
            fprintf(stderr, "Empty argument in function call on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
        }

        if (token.type == TYPE_COMMA)
        {
            token = newToken(0);
            if (token.type == TYPE_RBRACKET)
            {
                fprintf(stderr, "Empty argument in function call on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return (ERR_SYN);
            }
        }
        else if (token.type != TYPE_RBRACKET)
        {
            fprintf(stderr, "Expected \",\" or \")\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }
    }

    if (argCount < paramCount && paramCount != -1 && item->data.funData.defined == 1)
    {
        fprintf(stderr, "Too few arguments in function call on line %d!\n", token.rowNumber);
        makeError(ERR_RUNPAR);
        return ERR_RUNPAR;
    }
    else if (item->data.funData.defined == 0)
    {
        Token tmpToken;
        tmpToken.type = TYPE_INT;
        tmpToken.attribute.intV = argCount;
        STACK_push(notDefinedCalls, tmpToken);
    }

    token = funID;
    CODEfuncCall(getCode(isGlobal), funID, argCount);
    return 0;
}

/**
 * @brief semantics rules for expression
 *
 * @param ruleUsed index of used rule
 * @param tokenArr array of used tokens while reducing
 * @param endToken last legit token
 * @return ErrorType
 */
ErrorType rulesSematics(int ruleUsed, Token *tokenArr, Token endToken)
{
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
    CODEarithmetic(ruleUsed, tokenArr, endToken, isGlobal);
    return 0;
}



/**
 * @brief process expression
 *
 * @param isEmpty pointer, stores 1 if expression is empty
 * @param usePrevToken use - 1, dont use - 0
 * @return ErrorType
 */
ErrorType exprAnal(int *isEmpty, int usePrevToken)
{
    Token tmpToken, endToken = token;
    ErrorType err = 0;
    int done = 0;
    *isEmpty = 0;

    // init expression stack
    Stack *stack = STACK_init();
    tmpToken.type = TYPE_STACKEMPTY;
    STACK_push(stack, tmpToken);

    if (usePrevToken)
    {
        tmpToken.type = TYPE_LESSPREC;
        STACK_push(stack, tmpToken);
        STACK_push(stack, prevToken);

        // nothing to proccess after prevToken
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type))
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }
    }
    else
    {
        // empty expression
        if (!isOperatorType(token.type) && !isValueType(token.type) && token.type != TYPE_LBRACKET)
        {
            STACK_dispose(stack);
            *isEmpty = 1;
            return err;
        }
    }
    if (token.type == TYPE_FUNID)
    {
        err = functionCallCheckAndProcess();
    }

    while (1)
    {
        Token undefToken;
        undefToken.type = TYPE_UNDEF;
        Token tokenArrExpr[3] = {undefToken, undefToken, undefToken};
        int stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
        int tokenPrecIndex = getPrecTableIndex(token);

        if (STACK_top(stack)->type == TYPE_EXPR)
        {
            // load precedence of terminal
            STACK_pop(stack);
            stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }

        // precedence of token not found
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
            // if expression is on top, push < behind it
            if (isOperatorType(token.type) && STACK_top(stack)->type == TYPE_EXPR)
            {
                STACK_pop(stack);
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
                tmpToken.type = TYPE_EXPR;
                STACK_push(stack, tmpToken);
            }
            else // expression is not on top, push <
            {
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
            }

            // push new token
            STACK_push(stack, token);
            token = newToken(0);
            break;

        case R: // >
            // pop beteween < and >
            for (int i = 0; STACK_top(stack)->type != TYPE_LESSPREC; i++)
            {
                tokenArrExpr[i] = *STACK_top(stack);
                STACK_pop(stack);

                // array overflow
                if (i > 2)
                {
                    token = endToken;
                    fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
                    STACK_dispose(stack);
                    makeError(ERR_SYN);
                    return ERR_SYN;
                }
            }

            // check expression rules
            int usedRule = exprUseRule(tokenArrExpr);
            if (usedRule == -1)
            {
                if (token.type == TYPE_STACKEMPTY)
                {
                    token = endToken;
                }
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
            if (token.type == TYPE_RBRACKET)
            {
                return 0;
            }
            token = endToken;
            fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
            break;
        }

        if (token.type == TYPE_FUNID)
        {
            err = functionCallCheckAndProcess();
        }

        // non-expression token loaded
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type) && !done)
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }

        // expression is on top of the stack
        if (done && STACK_top(stack)->type == TYPE_EXPR)
        {
            STACK_pop(stack);
            // only TYPE_EXPR is on stack
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
        if (item->data.funData.defined == 0)
        {
            fprintf(stderr, "Function \"%s\" on line %d is not defined!\n", DS_string(funID.attribute.dString), funID.rowNumber);
            makeError(ERR_FUNDEF);
            return ERR_FUNDEF;
        }
        // check counts of arguments and paramaters
        argCount = strlen(item->data.funData.funTypes) - 1;
        STACK_popBottom(notDefinedCalls);
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
 * @brief main parser function
 *
 * @return int error code
 */
#ifdef scanner
int parser()
#else
int parser(Token *tokenArrIN)      // sim
#endif
{
#ifndef scanner
    tokenArr = tokenArrIN; // sim
#endif

    firstError = 0;
    globalST = ST_initTable(16);
    localST = NULL;
    isGlobal = 1;
    notDefinedCalls = STACK_init();
    functionTypes = DS_init();
    functionsCode.string = DS_init();
    functionsCode.lastUnconditionedLine = -1;
    progCode.string = DS_init();
    progCode.lastUnconditionedLine = -1;

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

#ifdef scanner
    if (firstError == 0)
    {
        printf("%s", DS_string(getCode(false)));
        printf("%s", DS_string(getCode(true)));
    }
#endif

    DS_dispose(functionsCode.string);
    DS_dispose(progCode.string);
    DS_dispose(functionTypes);
    ST_freeTable(globalST);
    // printf("Parser OK!\n");
    return firstError; // predelat na exit
}