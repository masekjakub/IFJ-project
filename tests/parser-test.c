/**
 * @file parser-test.c
 * @authors Jakub Mašek, Martin Zelenák
 * @brief test for parser.c
 * @version 0.1
 * @date 20-11-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "../parser.h"

#define TEST(NAME, DESCRIPTION)                  \
    void NAME()                                  \
    {                                            \
        printf("[%s] %s\n", #NAME, DESCRIPTION); \
        Token *tokensArr = (Token *)malloc(50 * sizeof(Token));\
        int returnedVal = 0;                     \
        tokenSimIndexReset();                    \

#define FAIL(REASON) printf("\x1b[31m[FAILED] %s\033[0m\n", REASON);
#define PASS() printf("\x1b[32m[PASSED]\033[0m\n");

#define ASSERT(EXPRESSION, REASON2FAIL, CODE)   \
    if (!(EXPRESSION))                          \
    {                                           \
        FAIL(REASON2FAIL);                      \
        printf("Returned: %d\n", CODE);         \
        TEST_NUM++;                             \
        return;                                 \
    }

#define ENDTEST         \
    free(tokensArr);    \
    PASS()              \
    SUCCESSFUL_TESTS++; \
    TEST_NUM++;         \
    }

#define ENDTEST_NORETURN\
    free(tokensArr);    \
    }                   \
    


#define PROLOG                                                      \
    makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);                \
    makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);           \
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

#define EPILOG                                              \
    makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);          \
    makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);          \

#define FUNCDEF(NAME)                                                     \
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);\
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, NAME);\
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);\
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);\
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);\
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);\
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);\
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);\
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);\
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);\
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);\

    unsigned int SUCCESSFUL_TESTS = 0;
    unsigned int TEST_NUM = 0;

    TEST(test_prolog1, "Prolog_OK")
    // prolog
    makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);

    EPILOG

    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_prolog2, "Prolog_ERR_COMMBEFORE")
    // prolog
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);

    EPILOG

    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == ERR_SYN, "Return code not ERR_SYN", returnedVal)
    ENDTEST

    TEST(test_epilog1, "Epilog_OK")
    PROLOG
    EPILOG

    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal);
    ENDTEST

    TEST(test_epilog2, "Epilog_OK - missing END")
    PROLOG

    // epilog
    makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code is not 0", returnedVal);
    ENDTEST

    TEST(test_epilog3, "Epilog_ERR - comment between END and EOF")
    PROLOG

    // epilog
    makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == ERR_SYN, "Return code is not SYN_ERR", returnedVal);
    ENDTEST

    TEST(test_assign, "$var = 5+8; OK")
    PROLOG
    //$var = 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 8, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_assign2, "$var = 5+8.8; OK")
    PROLOG
    //$var = 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FLOAT, 0, 0, 8.8, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_assign3, "$var = 5 / \"ahoj\"; OK")
    PROLOG
    //$var = 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_STRING, 0, 0, 0, "ahoj");
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_assign4, "$var = func(5) * func2(5); OK")
    PROLOG
    FUNCDEF("func")
    FUNCDEF("func2")
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "func");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "func2");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_add, "$var + 5;")
    PROLOG
    //$var + 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_add_nums, "5 + 5;")
    PROLOG
    //$var + 5;
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_add_brackets, "(5 + 5)+6;")
    PROLOG
    //$var + 5;TYPE_LBRACKET
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 6, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_id_wrong, "5.5 ! 5;")
    PROLOG
    //$var + 5;
    makeToken(tokensArr, TYPE_FLOAT, 0, 0, 5.5, NULL);
    makeToken(tokensArr, TYPE_NEG, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_id_wrong2, "9 * 5")
    PROLOG
    //$var + 5;
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_num_alone, "9;")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST
    
    TEST(test_var_alone, "$promenna1238984 = 5; $promenna1238984;")
    PROLOG
    // init var
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "promenna1238984");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    //call var
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "promenna1238984");
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_var_alone_notdef, "$promenna1238984; WRONG")
    PROLOG
    //call var
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "promenna1238984");
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == ERR_UNDEF, "Return code not 5", returnedVal)
    ENDTEST

    TEST(test_prec, "1+2*3; output: 0 14 0 12 0")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 3, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr, "1+(-2*3; WRONG")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_SUB, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 3, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_expr2, "(1-5)*(3+2); OK")
    PROLOG
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_SUB, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 3, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr3, "-1/3; WRONG")
    PROLOG
    makeToken(tokensArr, TYPE_SUB, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 3, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_expr4, "-*1/3; WRONG")
    PROLOG
    makeToken(tokensArr, TYPE_SUB, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 3, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_expr_comp, "1<2; OK")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_LESS, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr_comp2, "1<2 !== 5+5 >= 1/2; OK")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_LESS, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);

    makeToken(tokensArr, TYPE_NOTEQTYPES, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);

    makeToken(tokensArr, TYPE_GREATEREQ, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);

    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr_comp3, "1/($var !== 5)+5; OK")
    PROLOG

    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_NOTEQTYPES, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);

    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr_comp4, "1<=0.1 !== 5; OK")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_LESSEQ, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FLOAT, 0, 0, 0.1, "var");
    makeToken(tokensArr, TYPE_NOTEQTYPES, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);

    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_expr_comp5, "1===999.1 !== 5; WRONG")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_EQTYPES, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FLOAT, 0, 0, 999.1, "var");
    makeToken(tokensArr, TYPE_NOTEQTYPES, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);

    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_expr5, "1/+3); WRONG")
    PROLOG
    makeToken(tokensArr, TYPE_INT, 0, 1, 0, NULL);
    makeToken(tokensArr, TYPE_DIV, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 2, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_if_ok, "if_ok")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_IF, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_LESS, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, NULL);
    //if true part
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_ELSE, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, NULL);
    //else part
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_while, "while_ok")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_WHILE, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, NULL);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funccal, "funccal_ok")
    PROLOG
    FUNCDEF("fuction name")
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funccal2, "funccal_udef")
    PROLOG
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 5, "Return code not 5", returnedVal)
    ENDTEST

    TEST(test_funccal3, "funccal_notdef_param")
    PROLOG
    FUNCDEF("fuction name")
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_FLOAT, 0, 0, 0.1, NULL);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == ERR_RUNPAR, "Return code not 4", returnedVal)
    ENDTEST

    TEST(test_funcdef_ok, "funcdef_ok")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funcdef_ok2, "funcdef_ok with parametrs")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    //(<params>)
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_QMARK, 0, 0, 0, 0);               //?int $param1, string $param2
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "$param1");
    makeToken(tokensArr, TYPE_COMMA, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_STRING, 0, 0, 0);
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "$param2");
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    // : int {
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    //<stat_list>
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    //return 5 ; }
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_return_ok, "return_ok")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);

    //Non function return
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funcdef_wrong, "funcdef wrong")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    //!CHYBÍ! makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_funcdef_wrong2, "void funcdef with return value")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 6, "Return code not 6", returnedVal)
    ENDTEST

    TEST(test_funcdef_wrong3, "funcdef no return value")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    
    makeToken(tokensArr, TYPE_INT, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 6, "Return code not 6", returnedVal)
    ENDTEST

    TEST(test_funcdef_wrong4, "funcdef inside function")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    //inside function
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 3, "Return code not 3", returnedVal)
    ENDTEST

    TEST(test_funcdef_wrong5, "funcdef wrong params")
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction_name");
    //(<params>)
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_QMARK, 0, 0, 0, 0);               //?int $param1, string $param2
    //!CHYBÍ! makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "$param1");
    makeToken(tokensArr, TYPE_COMMA, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_STRING, 0, 0, 0);
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "$param2");
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);


    makeToken(tokensArr, TYPE_LBRACES, 0, 0, 0, 0);

    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_RETURN, 9, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 9, 0, NULL);
    makeToken(tokensArr, TYPE_RBRACES, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    int main()
    {
        printf("IFJ/IAL Project: Parser Tests\n");
        printf("================================================\n");

        test_prolog1();
        test_prolog2();
        test_epilog1();
        test_epilog2();
        test_epilog3();
        test_assign();
        test_add();
        test_add_nums();
        test_id_wrong();
        test_id_wrong2();
        test_num_alone();
        test_var_alone();
        test_var_alone_notdef();
        test_prec();
        test_expr();
        test_expr2();
        test_expr3();
        test_expr4();
        test_expr5();
        test_expr_comp();
        test_expr_comp2();
        test_expr_comp3();
        test_expr_comp4();
        test_expr_comp5();
        test_if_ok();
        test_while();
        test_funccal();
        test_funccal2();
        test_funccal3();
        test_funcdef_ok();
        test_funcdef_ok2();
        test_return_ok();
        test_funcdef_wrong();
        test_funcdef_wrong2();
        test_funcdef_wrong3();
        test_funcdef_wrong4();
        test_funcdef_wrong5();
        test_assign2();
        test_assign3();
        test_assign4();

            printf("================================================\n");
        float score = (float)SUCCESSFUL_TESTS / (float)TEST_NUM;
        if (score == 1.0)
        {
            printf("\x1b[32m[ALL TESTS PASSED]\033[0m\n");
            return 0;
    }
    if (score == 0.0)
    {
        printf("\x1b[31m[ALL TESTS FAILED]\033[0m\n");
        return 0;
    }
    printf("\033[93m[SCORE] %.1f%%\033[0m\n", score * 100);
    return 0;
}
