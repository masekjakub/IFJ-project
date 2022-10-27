ALL_FILES := parser.c getTokenSim.c scanner.c dynamicString.c symtable.c


all: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra $(ALL_FILES) -o translator

debug: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra -g $(ALL_FILES) -o parser

parser: $(ALL_FILES)
	gcc -std=c99 -Wall -Wextra $(ALL_FILES) -o parser

test-symtable:
	@gcc -std=c99 -Wall -Wextra tests/symtable-test.c symtable.c -o symtable-test
	@./symtable-test
	@rm symtable-test

test-ds:
	@gcc -std=c99 -Wall -Wextra tests/dynamicString-test.c dynamicString.c -o dynamicString-test
	@./dynamicString-test

clean:
	rm -f parser
	rm -f symtable-test
	rm -f dynamicString-test
	rm -f translator