ALL_C_FILES := parser.c scanner.c dynamicString.c symtable.c stack.c codeGenerator.c main.c getTokenSim.c
FLAGS := -std=c99 -Wall -Wextra

all: $(ALL_C_FILES)
	@gcc $(FLAGS) $(ALL_C_FILES) -o translator

debug: $(ALL_C_FILES)
	@gcc $(FLAGS) -g $(ALL_C_FILES) -o translator

parser: $(ALL_C_FILES)
	@gcc $(FLAGS) -g tests/parser-test.c parser.c getTokenSim.c symtable.c stack.c scanner.h dynamicString.c codeGenerator.c -o parser-test

scanner: $(ALL_C_FILES)
	@gcc $(FLAGS) main.c scanner.c dynamicString.c -o main
	@./main text.php
	@rm main

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
	@rm -f xmasek19.zip
	@rm -f *.o

pack:
	@zip xmasek19.zip *.c *.h dokumentace.pdf rozdeleni rozsireni Makefile

check:
	@bash tests/is_it_ok.sh xmasek19.zip testdir
	@rm -f xmasek19.zip
	@rm -rf testdir

