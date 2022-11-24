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

int firstError;     // first encountered error
Symtable *globalST; // global symtable
Symtable *localST;  // local symtable
int isGlobal;       // program is not in function
TokenType returnType = TYPE_VOID;   //Return type of currently parsed function
DynamicString *functionTypes;       //Return type and param types of currently parsed function
DynamicString *progCode;            //Program code

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

Token *tokenArr; // simulation

#define numOfExprRules 18
// rules are flipped because of stack
TokenType exprRules[numOfExprRules][3] = {
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
    {TYPE_EXPR, TYPE_CONCAT, TYPE_EXPR},        // E => E . E
    {TYPE_RBRACKET, TYPE_EXPR, TYPE_LBRACKET}, // E => (E)
    {TYPE_EXPR, TYPE_EQTYPES, TYPE_EXPR},    // E => E === E
    {TYPE_EXPR, TYPE_NOTEQTYPES, TYPE_EXPR}, // E => E !== E
    {TYPE_EXPR, TYPE_LESS, TYPE_EXPR},       // E => E < E
    {TYPE_EXPR, TYPE_GREATER, TYPE_EXPR},    // E => E > E
    {TYPE_EXPR, TYPE_LESSEQ, TYPE_EXPR},     // E => E <= E
    {TYPE_EXPR, TYPE_GREATEREQ, TYPE_EXPR}}; // E => E >= E

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

    if(token.type == TYPE_LEXERR){
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
    if(firstError == 0){
        firstError = err;
    }

    //Skipping section of code with error in it
    //while(token.type != TYPE_RBRACKET && token.type != TYPE_RBRACES && token.type != TYPE_SEMICOLON && token.type != TYPE_COMMA){
    while(token.type != TYPE_RBRACES && token.type != TYPE_LBRACES &&token.type != TYPE_SEMICOLON){
        if(token.type == TYPE_EOF) return;
        token = newToken(0);
    }
    //token = newToken(0);
    // exit(err);
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

/**
 * @brief fill ST with builtin functions info
 *
 * @param stable table to fill
 */
void builtInFuncFillST(Symtable *stable)
{
    STItemData data;
    data.funData.defined = 1;

    data.funData.funTypes = "U"; // unlimited
    ST_insertItem(stable, "write", ST_ITEM_TYPE_FUNCTION, data);

    data.funData.funTypes = "";// no parameters
    ST_insertItem(stable, "readi", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "readf", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "reads", ST_ITEM_TYPE_FUNCTION, data);

    data.funData.funTypes = "x"; // not defined type
    ST_insertItem(stable, "floatval", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "intval", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "strval", ST_ITEM_TYPE_FUNCTION, data);

    data.funData.funTypes = "s";
    ST_insertItem(stable, "strlen", ST_ITEM_TYPE_FUNCTION, data);
    ST_insertItem(stable, "ord", ST_ITEM_TYPE_FUNCTION, data);

    data.funData.funTypes = "i";
    ST_insertItem(stable, "chr", ST_ITEM_TYPE_FUNCTION, data);

    data.funData.funTypes = "sii";
    ST_insertItem(stable, "substring", ST_ITEM_TYPE_FUNCTION, data);
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

    token = newToken(1);
    while (token.type == TYPE_COMM)
    {
        token = newToken(1);
    }

    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    token = newToken(1);
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
        //if (err)
        //    return err;

        if(token.type == TYPE_LBRACES){
            openBracesBlocks++;
            token = newToken(0);
        }
        
        //<return>
        tmpErr = ruleReturn();
        if (!err)
        {
            err = tmpErr;
        }
        if(!err) err = tmpErr;

        // EPILOG
        if (token.type == TYPE_EOF)
            return err;

        if (token.type == TYPE_END)
        {
            token = newToken(1);
            if (token.type == TYPE_EOF)
            {
                return err;
            }
            fprintf(stderr, "Expected EOF after \"?>\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }

        // Check end of {<stat-list>}
        //if (token.type == TYPE_RBRACES) break;
        if (token.type == TYPE_RBRACES){
            openBracesBlocks--;
            if(openBracesBlocks < 0){
                fprintf(stderr, "Unexpected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return (ERR_SYN);
            }
            if(openBracesBlocks == 0) break;
            token = newToken(0);
        }

        // statement rule
        tmpErr = ruleStat();
        if(!err) err = tmpErr;
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
    char varType;

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

            // (<expr>)
            err = exprAnal(&varType, 0);

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

            // (<expr>)
            err = exprAnal(&varType, 0);

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
            break;

        case KEYWORD_FUNCTION:
            //Defining inside function
            if(!isGlobal){
                fprintf(stderr, "Definition of function inside function on line %d!\n", token.rowNumber);
                makeError(ERR_FUNDEF);
                while(token.type != TYPE_RBRACES){
                    token = newToken(0);
                }
                //token = newToken(0);
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
            //Check redefinition
            STItem *foundFunction = ST_searchTable(getTable(1),token.attribute.dString->string);
            if( foundFunction != NULL)
                if(foundFunction->type == ST_ITEM_TYPE_FUNCTION && foundFunction->data.funData.defined == true){
                    fprintf(stderr, "Redefinition of function \"%s\" on line %d!\n", token.attribute.dString->string, token.rowNumber);
                    makeError(ERR_FUNDEF);
                    return ERR_FUNDEF;
                    break;
                }
            DynamicString *funId;
            funId = DS_init();
            DS_appendString(funId, token.attribute.dString->string);
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

            // <params>
            ST_freeTable(localST);
            localST = ST_initTable(8);
            err = ruleParams();
            //Inserting function into symtable
            STItemData newFunData;
            newFunData.funData.defined = 1;
            newFunData.funData.funTypes = functionTypes->string;
            ST_insertItem(getTable(1), funId->string, ST_ITEM_TYPE_FUNCTION, newFunData);
            DS_dispose(functionTypes);
            functionTypes = DS_init();

            // )
            if (token.type != TYPE_RBRACKET)
            {
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <funcdef>
            errTmp = ruleFuncdef();
            if (err == 0)
            {
                err = errTmp;
            }
            
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
    isGlobal = 0;   //Set parsing in function
    returnType = TYPE_VOID; //Set unspecified return type

    // : ?TYPE
    if (token.type == TYPE_COLON)
    {
        token = newToken(0);
        
        // ?
        if(token.type == TYPE_QMARK){
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
        returnType = keywordType2VarType(token.attribute.keyword); // Set specified return type
        if(canBeNull) returnType -= 32; //Capitalize if function can return null or other type
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

    returnType = TYPE_VOID; //Set unspecified return type
    isGlobal = 1; //Set parsing in global scale

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
    char varType;

    // <assign> => <expr> ;
    if (token.type != TYPE_ID)
    {
        err = exprAnal(&varType, 0);
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
            err = exprAnal(&varType, 0);

            if (item == NULL){
                STItemData STdata;
                STdata.varData.VarType = varType;
                ST_insertItem(getTable(isGlobal),DS_string(prevToken.attribute.dString),ST_ITEM_TYPE_VARIABLE, data);
            }

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
            err = exprAnal(&varType, 1);
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
    // epsilon
    //Check empty parametr list
    if(token.type == TYPE_RBRACKET) return 0;

    ErrorType err = 0;
    ErrorType tmpErr = 0;
    
    // <param>
    err = ruleParam();

    // <params_2>
    tmpErr = ruleParams2();
    if(!err) err = tmpErr;

    return err;
}

/**
 * @brief params2 rule
 * 
 * @return ErrorType 
 */
ErrorType ruleParams2(){
    // epsilon
    //Check end of parametr list
    if(token.type == TYPE_RBRACKET) return 0;

    ErrorType err = 0;
    ErrorType tmpErr = 0;
    
    // ,
    if(token.type != TYPE_COMMA){
        fprintf(stderr, "Expected \",\" between parametrs of function on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    token = newToken(0);

    // <param>
    err = ruleParam();

    // <params_2>
    tmpErr = ruleParams2();
    if(!err) err = tmpErr;

    return err;
}

/**
 * @brief param rule
 * 
 * @return ErrorType 
 */
ErrorType ruleParam(){
    ErrorType err = 0;
    bool canBeNull = 0;
    char paramType = '!';

    if(token.type != TYPE_QMARK && token.type != TYPE_KEYWORD){
        fprintf(stderr, "Expected type of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    // ?
    if(token.type == TYPE_QMARK){
        canBeNull = true;
        token = newToken(0);
    }

    // TYPE
    if(token.type != TYPE_KEYWORD){
        err = 1;
    }
    else if(!isKeywordType(token.attribute.keyword)){
        err = 1;
    }
    if(err){
        fprintf(stderr, "Expected type of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    paramType = getTypeChar(keywordType2VarType(token.attribute.keyword));
    if(canBeNull) paramType -= 32; //Capitalize if param can be null
    token = newToken(0);

    // ID
    if(token.type != TYPE_ID){
        fprintf(stderr, "Expected a name of function parametr on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }
    //Check same name params
    if(ST_searchTable(getTable(0), token.attribute.dString->string) != NULL){
        fprintf(stderr, "Redefinition of function parametr \"%s\" on line %d!\n", token.attribute.dString->string, token.rowNumber);
        makeError(ERR_OTHER);
        return (ERR_OTHER);
    }
    //Insert param as local variable into symtable
    STItemData newVarData;
    newVarData.varData.VarType = paramType;
    ST_insertItem(getTable(0), token.attribute.dString->string, ST_ITEM_TYPE_VARIABLE, newVarData);
    //Adding param to functionTypes
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
        token = newToken(0);

        // Returns a value but is of type void
        if (returnType == TYPE_VOID && isGlobal == 0 && err == 0)
        {
            fprintf(stderr, "Function is of type void, but returns a value on line %d!\n", token.rowNumber);
            makeError(ERR_EXPRES);
            return (ERR_EXPRES);
        }

        return err;
    } // ;
    else if (returnType != TYPE_VOID && isGlobal == 0)
    { // Must be ";" | checking return value
        fprintf(stderr, "No return value in non-void function on line %d!\n", token.rowNumber);
        makeError(ERR_EXPRES);
        return (ERR_EXPRES);
    }
    token = newToken(0);

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
    STItem *item = ST_searchTable(getTable(1),DS_string(token.attribute.dString));
    if(item == NULL) // function not defined
    {
        fprintf(stderr, "Function \"%s\" on line %d is not defined!\n", DS_string(token.attribute.dString), token.rowNumber);
        makeError(ERR_UNDEF);
        return ERR_UNDEF;
    }
    int paramCount = -1;
    if(item->data.funData.funTypes[0] != 'U') //unlimited
        paramCount = strlen(item->data.funData.funTypes);
    token = newToken(0);

    // (
    if(token.type != TYPE_LBRACKET){
        fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    token = newToken(0);
    while (token.type != TYPE_RBRACKET)
    {
        if(isValueType(token.type)){
            argCount++;
            if(argCount > paramCount && paramCount != -1)
            {
                fprintf(stderr, "Too many arguments in function call on line %d!\n", token.rowNumber);
                makeError(ERR_RUNPAR);
                return ERR_RUNPAR;
            }
        }
        exprAnal(&isEmpty, 0);
        if(isEmpty){
            fprintf(stderr, "Empty expression in function call on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
        }

        //CODEpushValue(progCode, token);

        if(token.type == TYPE_COMMA)
            token = newToken(0);
        else if(token.type != TYPE_RBRACKET)
        {
            fprintf(stderr, "Expected \",\" or \")\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }
    }

    token = funID;
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
ErrorType rulesSematics(int ruleUsed, Token *tokenArr, Token endToken){
    if(ruleUsed == 0){
        STItem *item = ST_searchTable(getTable(isGlobal),DS_string(tokenArr[0].attribute.dString));
        if(item == NULL){
            token = endToken;
            fprintf(stderr, "Usage of not initialized variable \"%s\" on line %d!\n",DS_string(tokenArr[0].attribute.dString), token.rowNumber);
            makeError(ERR_UNDEF);
            return ERR_UNDEF;
        }
    }
    return 0;
}

/**
 * @brief process expression
 *
 * @param isEmpty pointer, stores if expression is empty
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
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type))
        {
            STACK_dispose(stack);
            return err;
        }

    }

    if(token.type == TYPE_FUNID){
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

            //push new token
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
            if (token.type == TYPE_RBRACKET){
                return 0;
            }
            token = endToken;
            fprintf(stderr, "Invalid expression on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
            break;
        }

        if(token.type == TYPE_FUNID){
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

/**
 * @brief main parser function
 *
 * @return int error code
 */
#ifdef scanner
int parser()
#else
int parser(Token *tokenArrIN) // sim
#endif
{
    #ifndef scanner
    tokenArr = tokenArrIN; // sim
    #endif

    firstError = 0;
    globalST = ST_initTable(16);
    localST = NULL;
    isGlobal = 1;
    returnType = TYPE_VOID;
    functionTypes = DS_init();
    DynamicString *functionsCode;
    functionsCode = DS_init();
    progCode = DS_init();
    DS_appendString(progCode, "LABEL @main\n");

    builtInFuncFillST(globalST);

    generateBuiltInFunc(functionsCode);
    //  <prog> => BEGIN DECLARE_ST <stat_list>
    ruleProg();

    #ifdef scanner
    printf("%s",DS_string(functionsCode));
    printf("%s",DS_string(progCode));
    #endif
    DS_dispose(functionsCode);
    DS_dispose(progCode);
    DS_dispose(functionTypes);
    ST_freeTable(globalST);
    // printf("Parser OK!\n");
    return firstError; // predelat na exit
}