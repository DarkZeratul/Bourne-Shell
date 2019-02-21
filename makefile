# Default rule to execute when the make command has no arguments
all: minishell

# Assembler Step: Produces the required object files.
main.o: main.c minishell_functions.h
	gcc -c main.c -o main.o -Wall -pedantic

minishell_functions.o: minishell_functions.h
	gcc -c minishell_functions.c -o minishell_functions.o -Wall -pedantic

# Linker Step: Produces the Final Executable File
minishell: main.o minishell_functions.o
	gcc main.o minishell_functions.o -o minishell -Wall -pedantic

# The cmd-line 'make clean' executes the following cmd
# Remover ALL files created during the previous steps.
clean: 
	rm -f minishell main.o minishell_functions.o

# Lists the "phony" rules in this file: 'all' and 'clean'
.PHONY: all clean
