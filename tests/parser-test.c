#include "../parser.h"

#define TEST(NAME, DESCRIPTION)                  \
    void NAME()                                  \
    {                                            \
        printf("[%s] %s\n", #NAME, DESCRIPTION); \
        Token *tokensArr = (Token *)malloc(50 * sizeof(Token)); // edit max size if needed!
        int returnedVal;

#define FAIL(REASON) printf("\x1b[31m[FAILED] %s\033[0m\n", REASON);
#define PASS() printf("\x1b[32m[PASSED]\033[0m\n");

#define ASSERT(EXPRESSION, REASON2FAIL, CODE)   \
    if (!(EXPRESSION))                          \
    {                                           \
        FAIL(REASON2FAIL);                      \
        printf("Returned: %d\n", CODE);         \
        return;                                 \
    }

#define ENDTEST         \
    free(tokensArr);     \
    PASS()              \
    SUCCESSFUL_TESTS++; \
    }

#define TEST_NUM 5

unsigned int SUCCESSFUL_TESTS = 0;

TEST(test_prolog1, "Prolog_OK")
// prolog
makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);

// epilog
makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

returnedVal = parser(tokensArr);
ASSERT(returnedVal == 0, "Return code not 0", returnedVal)
ENDTEST

TEST(test_prolog2, "Prolog_ERR_COMMBEFORE")
// prolog
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);

// epilog
makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

returnedVal = parser(tokensArr);
ASSERT(returnedVal == ERR_SYN, "Return code not ERR_SYN", returnedVal)
ENDTEST

TEST(test_epilog1, "Epilog_OK")
// prolog
makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);

// epilog
makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

returnedVal = parser(tokensArr);
ASSERT(returnedVal == 0, "Return code not 0", returnedVal);
ENDTEST

TEST(test_epilog2, "Epilog_OK - missing END")
// prolog
makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);

// epilog
makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

returnedVal = parser(tokensArr);
ASSERT(returnedVal == 0, "Return code is not 0", returnedVal);
ENDTEST

TEST(test_epilog3, "Epilog_ERR - comment between END and EOF")
// prolog
makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);

// epilog
makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);

returnedVal = parser(tokensArr);
ASSERT(returnedVal == ERR_SYN, "Return code is not SYN_ERR", returnedVal);
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
