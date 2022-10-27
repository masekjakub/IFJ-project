all:
	gcc -std=c99 -Wall -Wextra parser.c getTokenSim.c scanner.c dynamicString.c symtable.c -o parser
debug:
	gcc -std=c99 -Wall -Wextra -g parser.c getTokenSim.c scanner.c dynamicString.c -o parser
test-symtable:
	@gcc -std=c99 -Wall -Wextra tests/symtable-test.c symtable.c -o symtable-test
	@./symtable-test
	@rm symtable-test
test-ds:
	@gcc -std=c99 -Wall -Wextra tests/dynamicString-test.c dynamicString.c -o dynamicString-test
	@./dynamicString-test
clean:
	rm parser
	rm symtable-test