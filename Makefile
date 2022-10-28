ALL_FILES := parser.c getTokenSim.c scanner.c dynamicString.c symtable.c


all: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra $(ALL_FILES) -o translator

debug: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra -g $(ALL_FILES) -o parser

parser: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra parser.c getTokenSim.c scanner.h dynamicString.c symtable.c -o parser

scanner: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra scanner.c dynamicString.c -o scanner

test-symtable:
	@gcc -std=c99 -Wall -Wextra tests/symtable-test.c symtable.c -o symtable-test
	@./symtable-test
	@rm symtable-test

test-ds:
	@gcc -std=c99 -Wall -Wextra tests/dynamicString-test.c dynamicString.c -o dynamicString-test
	@./dynamicString-test

<<<<<<< Updated upstream
test-parser:
	@gcc -std=c99 -Wall -Wextra tests/parser-test.c parser.c getTokenSim.c symtable.c scanner.h dynamicString.c -o parser-test
	@./parser-test
	@rm parser-test
=======
test-stack:
	@gcc -std=c99 -Wall -Wextra tests/stack-test.c stack.c -o stack-test
	@./stack-test
>>>>>>> Stashed changes

clean:
	rm -f parser
	rm -f parser-test
	rm -f symtable-test
	rm -f dynamicString-test
	rm -f translator