/* Solver summary:
 * A module that is responsible for solving a sudoku board
 *
 * generateSolvedBoard: calls randomized backtrack function on a given board to create a solution
 * checkBlock: checks if a given "option" is valid for the given indices i,j. checking BLOCK only
 * checkRow: same as checkBlock only for the i'th row
 * checkCol: same as checkBlock only for the j'th column
 * backtrack: function to implement the backtrack algorithm. if RANDOM==1 it is randomized
 *			  otherwise it is deterministic.
 * removeOption: helper function for backtrack. given a pointer to a dynamic array of valid options,
 * 				 removes the option at the given index
 * */
#include "stdlib.h"
#include "stdio.h"
#include "Solver.h"
#include "main_aux.h"
#include "Stack.h"

/* num_solutions: a function that counts the number of possible solutions to the current board (if any) and prints it. the function runs iteratively, using a stack implementation
 * @param int* board: the game board
 * @param int ROWS: the number of rows in a block
 * @param int COLS: the number of cols in a block
 * @return: void
 * */
void num_solutions(int* board, int ROWS, int COLS){
	int totalRowLength = ROWS * COLS;
	int num_solutions = 0;
	int i=0, j=0, stack_index = 0;
	int return_flag = 0, direction = 1; /* return_flag is indicating whether the cell is preset and direction is back (exhausted forth), direction indicates which direction was last acted */
	STK_OBJ* tmp_stack_object;
	STK_OBJ** stack = (STK_OBJ**) malloc(totalRowLength*totalRowLength * sizeof(STK_OBJ*));
	/* in each iteration, i,j are indexes in the board, and option is the last option tried for this position, and fixed is flag for already full cells */
	do{
		/* filled the last cell */
		if(stack_index == totalRowLength * totalRowLength) {
			num_solutions++;
			tmp_stack_object = pop_stack(stack, &stack_index);
			return_flag = tmp_stack_object->preset;
			direction = 0;
			free(tmp_stack_object);
			j--;
		}
		/* returning through all preset cells to last changeable cell */
		if(return_flag){
			tmp_stack_object = pop_stack(stack, &stack_index);
			if(tmp_stack_object == NULL){
				break;
			}
			return_flag = tmp_stack_object->preset;
			free(tmp_stack_object);
			if(j){
				j--;
			} else {
				i--;
				j=totalRowLength-1;
			}
			continue;
		}
		/* on return to changeable cell */
		if(!direction || (direction && !board[i*totalRowLength + j])) {
			board[i*totalRowLength + j]++;
			/* cell is exhausted */
			if(board[i*totalRowLength + j] == totalRowLength+1) {
				board[i*totalRowLength + j] = 0;
				tmp_stack_object = pop_stack(stack, &stack_index);
				if(tmp_stack_object == NULL){
					break;
				}
				return_flag = tmp_stack_object->preset;
				direction = 0;
				free(tmp_stack_object);
				if(j){
					j--;
				} else {
					i--;
					j=totalRowLength-1;
				}
				continue;
				/* advance cell value */
			} else {
				if(checkRow(board, i, j, board[i*totalRowLength+j], ROWS, COLS)
						&& checkCol(board, i, j, board[i*totalRowLength+j], ROWS, COLS)
						&& checkBlock(board, i, j, board[i*totalRowLength+j], ROWS, COLS)){
					tmp_stack_object = create_object(0);
					push_stack(stack, &stack_index, tmp_stack_object);
					direction = 1;
					if(i==totalRowLength-1 || j<totalRowLength-1){
						j++;
					} else {
						i++;
						j=0;
					}
				} else {
					direction = 0;
				}
			}

		}
		/* on arrive to new cell while dir==1 (advancing not returning) */
		else {
			/* if cell is full, set as preset and advance */
			if(board[i*totalRowLength + j]) {
				tmp_stack_object = create_object(1);
				push_stack(stack, &stack_index, tmp_stack_object);
				if(i==totalRowLength-1 || j<totalRowLength-1){
					j++;
				} else {
					i++;
					j=0;
				}
			}
		}
	}while(1);
	printf("The number of possible solutions is: %d\n", num_solutions);
	free(stack);
}

