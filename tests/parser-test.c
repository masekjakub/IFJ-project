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

    TEST(test_assign3, "$var = 5 / \"ahoj\"; WRONG")
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
    ASSERT(returnedVal == 7, "Return code not 7", returnedVal)
    ENDTEST

    TEST(test_assign4, "$var = func * func; WRONG")
    PROLOG
    //$var = 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "func");
    makeToken(tokensArr, TYPE_MUL, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "func");
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 7, "Return code not 7", returnedVal)
    ENDTEST

    TEST(test_add, "$var + 5;")
    PROLOG
    //$var + 5;
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

    TEST(test_id_wrong, "$var ! 5;")
    PROLOG
    //$var + 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
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
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 2, "Return code not 2", returnedVal)
    ENDTEST

    TEST(test_if_ok, "if_ok") //dodelat pro cely if
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_IF, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_while, "while_ok") // dodelat pro cely while
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_WHILE, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funccal, "funccal_ok") // dodelat pro cele
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
    ENDTEST

    TEST(test_funcdef, "funcdef_ok") // dodelat pro cele
    PROLOG
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_FUNCTION, 0, 0, 0);
    makeToken(tokensArr, TYPE_FUNID, 0, 0, 0, "fuction name");
    makeToken(tokensArr, TYPE_LBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_RBRACKET, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_COLON, 0, 0, 0, 0);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_INT, 0, 0, 0);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, 0);
    EPILOG
    returnedVal = parser(tokensArr);
    ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
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
        test_if_ok();
        test_while();
        //test_funccal();
        //test_funcdef();
        test_assign2();
        //test_assign3();
        //test_assign4();

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
