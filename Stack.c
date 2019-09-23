
/* Stack module summary
 * holds all the functions necessary for the Stack Class.
 * pop_stack: extract the top STK_OBJ element from the stack, and return it to the caller
 *
 * push_stack: function adds a STK_OBJ pointer of a given STK_OBJ to the stack
 *
 * create_object: a helper function that creates an object with the relevant data - int (acting as boolean)
 */

#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

/*
 * pop_stack: extract the top STK_OBJ element from the stack, and return it to the caller
 * @param STK_OBJ** stack: pointer to the stack of STK_OBJ pointers
 * @param int* stack_index: pointer to the index of the stack
 * @return: STK_OBJ*
 */
STK_OBJ* pop_stack(STK_OBJ** stack, int* stack_index){
	if(*stack_index == 0){
		return NULL;
	}
	return stack[--(*stack_index)];
}
/*
 * push_stack: function adds a STK_OBJ pointer of a given STK_OBJ to the stack
 * @param STK_OBJ** stack: pointer to the stack of STK_OBJ pointers
 * @param int* stack_index: pointer to the index of the stack
 * @param STK_OBJ* object: pointer to the added to stack element
 * @return: void
 */
void push_stack(STK_OBJ** stack, int* stack_index, STK_OBJ* object){
	stack[(*stack_index)++] = object;
}

/*
 * create_object: a helper function that creates an object with the relevant data - int (acting as boolean)
 * @param int present: a number (acting as boolean) indicating whether the relevant index in the board points to a cell that was filled pre-test of num_solutions
 * @return: STK_OBJ*
 */
STK_OBJ* create_object(int preset){
	STK_OBJ* result = (STK_OBJ*) malloc(sizeof(STK_OBJ));
	result->preset = preset;
	return result;
}
