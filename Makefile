# @file Makefile
# @brief Makefile for IFJ project
# @author Jakub Mašek
# @date 2022-12-6

ALL_C_FILES := parser.c scanner.c dynamicString.c symtable.c stack.c codeGenerator.c main.c
ALL_H_FILES := parser.h scanner.h dynamicString.h symtable.h stack.h codeGenerator.h
FLAGS := -std=c99 -Wall -Wextra
LIBS := -lm

all: $(ALL_C_FILES) $(ALL_H_FILES)
	@gcc $(FLAGS) $(ALL_C_FILES) -o IFJ22 $(LIBS)

debug: $(ALL_C_FILES) $(ALL_H_FILES)
	@gcc $(FLAGS) -g $(ALL_C_FILES) -o IFJ22 $(LIBS)

parser: $(ALL_C_FILES) $(ALL_H_FILES) simulation/getTokenSim.h
	@gcc $(FLAGS) -g simulation/getTokenSim.c tests/parser-test.c parser.c symtable.c stack.c scanner.h dynamicString.c codeGenerator.c -o parser-test $(LIBS)

scanner: $(ALL_C_FILES) $(ALL_H_FILES)
	@gcc $(FLAGS) main.c scanner.c dynamicString.c -o main $(LIBS)
	@./main text.php
	@rm main

test-scanner:
	@gcc $(FLAGS) tests/scanner-test.c scanner.c dynamicString.c -o scanner-test $(LIBS)
	@./scanner-test text.php
	@rm scanner-test

test-symtable:
	@gcc $(FLAGS) -g tests/symtable-test.c symtable.c -o symtable-test $(LIBS)
	@./symtable-test
	@rm symtable-test

test-ds:
	@gcc $(FLAGS) tests/dynamicString-test.c dynamicString.c -o dynamicString-test $(LIBS)
	@./dynamicString-test

test-parser: $(ALL_C_FILES) $(ALL_H_FILES) simulation/getTokenSim.h
	@gcc $(FLAGS) tests/parser-test.c parser.c simulation/getTokenSim.c symtable.c stack.c scanner.h dynamicString.c codeGenerator.c -o parser-test $(LIBS)
	@./parser-test
	@rm parser-test
	
test-stack:
	@gcc $(FLAGS) tests/stack-test.c stack.c -o stack-test $(LIBS)
	@./stack-test

clean:
	@rm -f parser
	@rm -f parser-test
	@rm -f symtable-test
	@rm -f stack-test
	@rm -f dynamicString-test
	@rm -f IFJ22
	@rm -f xmasek19.zip
	@rm -f *.o

pack:
	@zip xmasek19.zip *.c *.h dokumentace.pdf rozdeleni rozsireni Makefile

#test of pack
check:
	@bash tests/is_it_ok.sh xmasek19.zip testdir
	@rm -f xmasek19.zip
	@rm -rf testdir

