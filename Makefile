ALL_FILES := parser.c scanner.c dynamicString.c symtable.c stack.c codeGenerator.c main.c
FLAGS := -std=c99 -Wall -Wextra

all: $(ALL_FILES)
	@gcc  $(FLAGS) $(ALL_FILES) -o translator

debug: $(ALL_FILES)
	@gcc $(FLAGS) -g $(ALL_FILES) -o translator

parser: $(ALL_FILES)
	@gcc $(FLAGS) -g  tests/parser-test.c parser.c getTokenSim.c symtable.c stack.c scanner.h dynamicString.c -o parser-test

scanner: $(ALL_FILES)
	@gcc $(FLAGS) main.c scanner.c dynamicString.c -o main

test-scanner: 
	@gcc $(FLAGS) tests/scanner-test.c scanner.c dynamicString.c -o scanner-test
	@./scanner-test text.php
	@rm scanner-test

test-symtable:
	@gcc $(FLAGS) -g tests/symtable-test.c symtable.c -o symtable-test
	@./symtable-test
	@rm symtable-test

test-ds:
	@gcc $(FLAGS) tests/dynamicString-test.c dynamicString.c -o dynamicString-test
	@./dynamicString-test

test-parser:
	@gcc $(FLAGS) tests/parser-test.c parser.c getTokenSim.c symtable.c stack.c scanner.h dynamicString.c codeGenerator.c -o parser-test
	@./parser-test
	@rm parser-test
	
test-stack:
	@gcc $(FLAGS) tests/stack-test.c stack.c -o stack-test
	@./stack-test

clean:
	@rm -f parser
	@rm -f parser-test
	@rm -f symtable-test
	@rm -f dynamicString-test
	@rm -f translator
	@rm -f *.o