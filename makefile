CC = gcc
OBJS = main.o Game.o Solver.o Parser.o DoublyLinkedList.o Error_Print.o Gurobi_Code.o main_aux.o Parser_aux.o Stack.o Commands.o Commands_aux.o
EXEC = sudoku
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Game.h Solver.h Parser.h main_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Solver.h Game.h Commands_aux.h main_aux.h Parser.h DoublyLinkedList.h Gurobi_Code.h Error_Print.h Stack.h Commands.h
	$(CC) $(COMP_FLAG) -c $*.c
Solver.o: Solver.h main_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.h main_aux.h Parser_aux.h Stack.h
	$(CC) $(COMP_FLAG) -c $*.c
main_aux.o: main_aux.h Error_Print.h
	$(CC) $(COMP_FLAG) -c $*.c
DoublyLinkedList.o: Commands_aux.h DoublyLinkedList.h 
	$(CC) $(COMP_FLAG) -c $*.c
Error_Print.o: Error_Print.h Parser_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
Gurobi_Code.o: Gurobi_Code.h Solver.h main_aux.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
Parser_aux.o: Parser_aux.h Error_Print.h
	$(CC) $(COMP_FLAG) -c $*.c
Stack.o: Stack.h
	$(CC) $(COMP_FLAG) -c $*.c
Commands.o: Game.h main_aux.h Parser.h Parser_aux.h DoublyLinkedList.h Gurobi_Code.h Error_Print.h Commands.h Commands_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
Commands_aux.o: main_aux.h DoublyLinkedList.h Error_Print.h Commands_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
all: $(EXEC)


clean:
	rm -f $(OBJS) $(EXEC)
