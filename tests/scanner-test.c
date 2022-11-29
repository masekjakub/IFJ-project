/**
 * @file scanner-test.c
 * @author Filip Polomski
 * @brief test for scanner for IFJ22 translator
 * @version 1.0
 * @date 20-11-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST(NAME, DESCRIPTION)													\
void NAME() {		                                                            \
    TEST_NUM++;															        \
	printf("[%s] %s\n", #NAME, DESCRIPTION);									\
    FILE *file = fopen("text.php", "w");                                        


#define FAIL(REASON) printf("\x1b[31m[FAILED] %s\033[0m\n", REASON);
#define PASS() printf("\x1b[32m[PASSED]\033[0m\n");

#define ASSERT(EXPRESSION, CODE)									            \
if(!(EXPRESSION)){																\
	/*ST_freeTable(table);*/													\
	FAIL("neco spatne");                                                      \
    printf("Returned: %d\n", CODE);												\
    return;																		\
}

#define ENDTEST																	\
	/*Try to free*/																\
	fclose(file);														        \
	PASS()																		\
	SUCCESSFUL_TESTS++;															\
}

unsigned int SUCCESSFUL_TESTS = 0;
unsigned int TEST_NUM = 0;

Token token;

// Tests for prolog
TEST(test_prolog, "Prolog: \"<?php \"")
    fprintf(file, "<?php ");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_BEGIN, token.type);
    getToken();
ENDTEST

TEST(test_wrong_prolog1, "Prolog: \"     <?php \"")
    fprintf(file, "   <?php ");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

TEST(test_wrong_prolog2, "Prolog: \"<?phph\"")
    fprintf(file, "<?phph");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

// Tests for commentary
TEST(test_commentary_in_line, "Commentary: //*&^$#@!)(;:\"\"")
    fprintf(file, "//*&^$#@!)(;:\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_COMM, token.type);
ENDTEST

TEST(test_block_commentary, "Commentary: /****  dsfsd/f546&^/645  **645***/")
    fprintf(file, "/****  dsfsd/f546&^/645  **645***/\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_COMM, token.type);
ENDTEST

TEST(test_wrong_block_commentary, "Commentary: /****  dsfsd/f546&^/645  **645***/")
    fprintf(file, "/****  dsfsd/f546&^/645  **645***\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

// Tests for string
TEST(test_right_string_type, "String: \"Hello World! 234&*)@$#^^$\"")
    fprintf(file, "\"Hello World! 234&*)@$#^^$\"\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_STRING, token.type);
ENDTEST

TEST(test_right_string, "String: \"Hello World! 234&*)@$#^^$\"\\n")
    fprintf(file, "\"Hello World! 234&*)@$#^^$\"\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_STRING, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "Hello World! 234&*)@$#^^$"), token.type);
ENDTEST

TEST(test_wrong_string, "String: \"Hello World! 234&*)@$#^^$\\n")
    fprintf(file, "\"Hello World! 234&*)@$#^^$\\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

TEST(test_empty_string, "\"\"")
    fprintf(file, "\"\"\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(!strcmp(DS_string(token.attribute.dString), "\0"), token.type);
ENDTEST

// Tests for variables
TEST(test_right_variable1, "Variable: $variable1_")
    fprintf(file, "$variable1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_ID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "$variable1_"), token.type);
ENDTEST

TEST(test_right_variable2, "Variable: $_variable1_")
    fprintf(file, "$_variable1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_ID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "$_variable1_"), token.type);
ENDTEST

TEST(test_right_variable3, "Variable: $Variable1_")
    fprintf(file, "$Variable1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_ID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "$Variable1_"), token.type);
ENDTEST

TEST(test_wrong_variable, "Variable: $1variable1_")
    fprintf(file, "$1variable1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

// Tests for declare(strict_types=1)
TEST(test_right_dec_str_types1, "declare(strict_types=1)")
    fprintf(file, "declare(strict_types=1)\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_DECLARE_ST, token.type);
ENDTEST

TEST(test_right_dec_str_types2, "declare(          strict_types      =     1                )")
    fprintf(file, "declare          (          strict_types      =     1                )\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    //printf("%s", DS_string(token.attribute.dString));
    ASSERT(token.type == TYPE_DECLARE_ST, token.type);
ENDTEST

TEST(test_wrong_dec_str_types1, "declare(strict_types1)")
    fprintf(file, "declare(strict_types1)\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

TEST(test_wrong_dec_str_types2, "declare(strict_types=1")
    fprintf(file, "declare(strict_types=1\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

// Tests for functions
TEST(test_right_function1, "Function: function1_")
    fprintf(file, "function1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "function1_"), token.type);
ENDTEST

TEST(test_right_function2, "Function: _function1_")
    fprintf(file, "_function1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "_function1_"), token.type);
ENDTEST

TEST(test_right_function3, "Function: Function1_")
    fprintf(file, "Function1_\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID, token.type);
    ASSERT(!strcmp(DS_string(token.attribute.dString), "Function1_"), token.type);
ENDTEST

// Tests for integer
TEST(test_right_integer1, "Integer: 12")
    fprintf(file, "12\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_INT, token.type);
    ASSERT(token.attribute.intV == 12, token.type);
ENDTEST

TEST(test_right_integer2, "Integer: -12")
    fprintf(file, "-12\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_SUB, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_INT, token.type);
    ASSERT(token.attribute.intV == 12, token.type);
ENDTEST

TEST(test_right_integer3, "Integer: +12")
    fprintf(file, "+12\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_ADD, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_INT, token.type);
    ASSERT(token.attribute.intV == 12, token.type);
ENDTEST

// Test for float
TEST(test_right_float1, "Float: 12.000000")
    fprintf(file, "12.000000\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FLOAT, token.type);
    ASSERT(token.attribute.doubleV == 12.000000, token.type);
ENDTEST

TEST(test_right_float2, "Float: 12.e2")
    fprintf(file, "12.e2\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FLOAT, token.type);
    ASSERT(token.attribute.doubleV == 1200.000000, token.type);
ENDTEST

TEST(test_right_float3, "Float: 12.E2")
    fprintf(file, "12.E2\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FLOAT, token.type);
    ASSERT(token.attribute.doubleV == 1200.000000, token.type);
ENDTEST

TEST(test_right_float4, "Float: 12.e-2")
    fprintf(file, "12.e-2\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FLOAT, token.type);
    ASSERT(token.attribute.doubleV == 0.120000, token.type);
ENDTEST

TEST(test_right_float5, "Float: 12.e+2")
    fprintf(file, "12.e+2\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_FLOAT, token.type);
    ASSERT(token.attribute.doubleV == 1200.000000, token.type);
ENDTEST

TEST(test_wrong_float, "Float: 12.")
    fprintf(file, "12.\n");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_LEXERR, token.type);
ENDTEST

// TODO float testy pro nespravne floaty

// Test with whole programs
TEST(test_program1, "Test program 1")
    fprintf(file, "<?php \n                                     \
        declare(strict_types=1);\n                              \
        function concat(string $x, string $y) : string{\n       \
            $x = $x . $y;\n                                     \
            return $x . \" \" . $y;\n                           \
        }\n                                                     \
        $a = \"ahoj \";\n                                       \
        $ret = concat($a, \"svete\");\n                         \
        write($ret, $a);\n                                      \
        ?>");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_BEGIN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_DECLARE_ST, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_FUNCTION, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "concat"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACES, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_RETURN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), " "), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACES, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), "ahoj "), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$ret"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "concat"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), "svete"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "write"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$ret"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_END, token.type);
ENDTEST

TEST(test_program1_one_line, "Test program 1")
    fprintf(file, "<?php declare(strict_types=1);function concat(string$x,string$y):string{$x=$x.$y;return $x.\" \".$y;}$a=\"ahoj \";$ret=concat($a,\"svete\");write($ret,$a);?>");
    fclose(file);
    file = fopen("text.php", "r");
    setSourceFile(file);
    token = getToken();
    ASSERT(token.type == TYPE_BEGIN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_DECLARE_ST, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_FUNCTION, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "concat"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_STRING, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACES, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_KEYWORD && token.attribute.keyword == KEYWORD_RETURN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$x"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), " "), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_CONCAT, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$y"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACES, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), "ahoj "), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$ret"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ASSIGN, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "concat"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_STRING && !strcmp(DS_string(token.attribute.dString), "svete"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_FUNID && !strcmp(DS_string(token.attribute.dString), "write"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_LBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$ret"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_COMMA, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_ID && !strcmp(DS_string(token.attribute.dString), "$a"), token.type);
    token = getToken();
    ASSERT(token.type == TYPE_RBRACKET, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_SEMICOLON, token.type);
    token = getToken();
    ASSERT(token.type == TYPE_END, token.type);
ENDTEST

int main(){
    // Tests for prolog
    test_prolog();
    test_wrong_prolog1();
    test_wrong_prolog2();

    // Tests for commentary
    test_commentary_in_line();
    test_block_commentary();
    test_wrong_block_commentary();

    // Tests for string
    test_right_string_type();
    test_right_string();
    test_wrong_string();
    test_empty_string();

    // Tests for variables
    test_right_variable1();
    test_right_variable2();
    test_right_variable3();
    test_wrong_variable();

    // Tests for declare(strict_types=1)
    test_right_dec_str_types1();
    test_right_dec_str_types2();
    test_wrong_dec_str_types1();
    test_wrong_dec_str_types2();

    // Tests for functions
    test_right_function1();
    test_right_function2();
    test_right_function3();

    // Tests for integer
    test_right_integer1();
    test_right_integer2();
    test_right_integer3();

    // Tests for float
    test_right_float1();
    test_right_float2();
    test_right_float3();
    test_right_float4();
    test_right_float5();
    test_wrong_float();

    // Test programs
    test_program1();
    test_program1_one_line();

    printf("================================================\n");
	float score = (float)SUCCESSFUL_TESTS/(float)TEST_NUM;
	if(score == 1.0){
		printf("\x1b[32m[ALL TESTS PASSED]\033[0m\n");
		return 0;
	}
	if(score == 0.0){
		printf("\x1b[31m[ALL TESTS FAILED]\033[0m\n");
		return 0;
	}
	printf("\033[93m[SCORE] %.1f%%\033[0m\n",score*100);
	return 0;
}