all:
	gcc -std=c99 -Wall -Wextra parser.c getTokenSim.c scanner.c dynamicString.c symtable.c -o  parser
debug:
	gcc -std=c99 -Wall -Wextra -g  parser.c getTokenSim.c scanner.c dynamicString.c symtable.c -o  parser
clean:
	rm parser