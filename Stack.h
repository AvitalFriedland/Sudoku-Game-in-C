/* Stack module summary
 * holds all the functions necessary for the Stack Class.
 * pop_stack: extract the top STK_OBJ element from the stack, and return it to the caller
 *
 * push_stack: function adds a STK_OBJ pointer of a given STK_OBJ to the stack
 *
 * create_object: a helper function that creates an object with the relevant data - int (acting as boolean)

 */

#ifndef STACK_H_
#define STACK_H_


typedef struct stack_object{
	int preset;
} STK_OBJ;

STK_OBJ* create_object(int preset);
void push_stack(STK_OBJ** stack, int* stack_index, STK_OBJ* object);
STK_OBJ* pop_stack(STK_OBJ** stack, int* stack_index);


#endif /* STACK_H_ */
