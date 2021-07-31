
##=========================================================================================================
## File:        Makefile
## Case:        VUT, FIT, IFJ, compiler project
## Date:        9. 12. 2020
## Author(s):   David Mihola
## Contac(s):   xmihol00@stud.fit.vutbr.cz
## Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
## Description: File, which builds the project. Creates an executable file called compiler. 
##==========================================================================================================

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
OBJ = symtable.o vector.o parser.o memory.o scanner.o oRPN.o token.o generator.o semantic.o optimizer.o register.o main.o functions.o
PROGRAM = compiler

.PHONY: all clean

all: $(PROGRAM)
$(PROGRAM): $(OBJ) $(OBJ_FAST)
	$(CC) $(CFLAGS) $(OBJ) -o $(PROGRAM)

%.o: %.c %.h %_private.h enums.h
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.c %.h enums.h
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	@rm $(OBJ) $(PROGRAM) 2>/dev/null || true
