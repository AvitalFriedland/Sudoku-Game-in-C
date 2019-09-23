/*
 *  Commands_aux.h summary:
 * A module that is responsible for handling the flow of a game
 *
 * board_load_value: helper function to load a value into board and\or fixed_board at board_index from the int-array three_last_chars
 * undo_redo_moves: function to undo or redo a move with moves_list. passes the state to "set" function.
 * checkDimensions: checks if dimensions are legal.
 * increase_indices: increase the row and col index while they are less than the dimensions of the board.
 * add_to_redo_list: adds a move to the redo list- compares the original board and the board_after_changes.
 * set: sets a value into the board (according to the state and whether it is fixed)
 * count_legal_options_cell: count the legal options for a cell.
 * legal_value_in_position: returns 1 if the value is legal for the given position. 0 otherwise.
 * clear_y_cells: used by Generate command- clears all but y random cells.
 * init_empty_board: initialize an empty 3x3 board- used by Edit command.
 */
#include <stdlib.h>
#include <stdio.h>
#include "Commands_aux.h"
#include "Error_Print.h"
#include "DoublyLinkedList.h"
#include "main_aux.h"

/* board_load_value: helper function to load a value into board and\or fixed_board at board_index from the int-array three_last_chars
 * @param int* board: the game board.
 * @param int* fixed_board: contains only fixed cells.
 * @param int board_index: the index into which to load the value.
 * @param int* three_last_chars: the int array which contains the wanted value to load.
 * @param int int i: decides how many digits are in the number.
 * @param int flag_fixed: a flag to indicate whether it is a fixed cell to load or not.
 * @return: void
 */
void board_load_value(int* board, int* fixed_board, int board_index,
		int* three_last_chars, int i, int flag_fixed) {
	int value;
	if (i == 1) {
		value = three_last_chars[0] - '0';
	} else {
		value = 10 * (three_last_chars[0] - '0') + (three_last_chars[1] - '0');
	}
	/* 	printf("value:%d ",value); */

	board[board_index] = value;
	/*	printf("board[index]:%d ",board[board_index]); */

	if (flag_fixed) {
		fixed_board[board_index] = value;
	} else {
		fixed_board[board_index] = 0;
	}

}


/* checkDimensions: checks if dimensions are legal.
 * @param int* dimensions: array with two dimensions.
 * @return: 1 if dimensions are legal 0 otherwise.
 */
int checkDimensions(int* dimensions) {
	return (dimensions[0] * dimensions[1]) < 100;
}

/* increase_indices: increase the row and col index while they are less than the dimensions of the board.
 * @param int* board_index: absolute index in a flattened-out array (not two dimensional).
 * @param int* col_index: the column index.
 * @param int* row_index: the row index.
 * @param int totalRowLength: total size of a row in the board.
 * @return: return 0 if the changes were successful, -1 if it is outside the bound of the board.
 *
*/
int increase_indices(int* board_index, int* col_index, int* row_index,
		int totalRowLength) {
	/*		printf("increasing indices:  row: %d, col: %d \n", row_index, col_index); */
	(*board_index)++;
	if ((*col_index) < totalRowLength) {
		(*col_index)++;
		return 0;
	} else {
		if (*row_index < totalRowLength) {
			(*col_index) = 1;
			(*row_index)++;
			return 0;
		}
		if (*row_index == totalRowLength) {
			(*row_index)++;
			return 0;
		}
		return -1;
	}
}

/* add_to_redo_list: adds a move to the redo list- compares the original board and the board_after_changes.
 * @param int* original_board: the board before the changes.
 * @param int* board_after_changes: the board after the changes.
 * @param NODE** current_list_ptr: a pointer to the head of the linked list.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @param int move_count: if this is given, there is no need to cound ahead of time the amount of moves to add to moves_list.
 * @return: void.
 *
*/
void add_to_redo_list(int* original_board, int* board_after_changes,NODE** current_list_ptr, int ROWS, int COLS, int move_count){
	MOVE* set_move =(MOVE*) malloc(sizeof(MOVE));

	MOVE* moves;
	int index;
	int i,j;
	int totalRowLength = ROWS*COLS;

	if (set_move == NULL ) {
			printFunctionError("malloc");
			exit(1);
		}
	if(move_count== 0){
		for (i = 0; i < totalRowLength; i++) {
			for (j = 0; j < totalRowLength; j++) {
				if (original_board[i * totalRowLength + j]
				          != board_after_changes[i * totalRowLength + j]) {
							move_count++;
	}
			}
		}
	}

	moves = (MOVE*) malloc(move_count * sizeof(MOVE));

	if (moves == NULL ) {
		printFunctionError("malloc");
		exit(1);

	}
	index = 0;
	for (i = 0; i < totalRowLength; i++) {
		for (j = 0; j < totalRowLength; j++) {
			if (board_after_changes[i * totalRowLength + j]!= original_board[i * totalRowLength + j]) {
				set_move = (MOVE*) realloc(set_move,sizeof(MOVE));
				set_move->x = j + 1;
				set_move->y = i + 1;
				set_move->after = board_after_changes[(i) * ROWS * COLS + (j)];
				set_move->before = original_board[(i) * ROWS * COLS + (j)];
				moves[index++] = *set_move;
			}
		}
	}

	(*current_list_ptr) = add_after((*current_list_ptr), moves, move_count);
	free(set_move);
}

/* undo_redo_moves: function to undo or redo a move with moves_list. passes the state to "set" function.
 * @param int undo_redo: flag to indicate- if -1 undo, if 1 redo.
 * @param MOVE* moves_list: the list with the moves to undo\redo.
 * @param int num_moves: number of moves in total to undo\redo.
 * @param int* board: the game board.
 * @param int* fixed_board: contains only fixed cells.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @param GAME_STATE state: the state of the board.
 * @return: void.
*/
void undo_redo_moves(int undo_redo, MOVE* moves_list, int num_moves,int* board, int* fixed_board, int ROWS, int COLS, GAME_STATE state){
	int i=0;
	MOVE cur;
	if(moves_list !=NULL ){
		for(i=0;i<num_moves;i++){
			cur = moves_list[i];

			if(undo_redo==-1){
				printf("un-doing move: setting value %d to <%d,%d> \n", cur.before, cur.x, cur.y);
				set(board, fixed_board,  cur.x -1, cur.y-1, cur.before, ROWS, COLS, state);}
			else if(undo_redo==1){
				printf("re-doing move: setting value %d to <%d,%d> \n", cur.after, cur.x, cur.y);
				set(board, fixed_board,  cur.x -1, cur.y-1, cur.after, ROWS,
						COLS,state);}
		}
	}
}


/* set: sets a value into the board (according to the state and whether it is fixed)
 * @param int* board: the game board.
 * @param int* fixed_board: contains only fixed cells.
 * @param int x: col index.
 * @param int y: row index.
 * @param int value: value to set.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @param GAME_STATE state: the state of the board.
 * @return: -1 if the set was unsuccessful, 0 if it was successful.
*/

int set(int* board, int* fixedBoard, int x, int y, int value, int ROWS, int COLS, GAME_STATE state) {

	int row, col, index;

	row = y;
	col = x;

	index = row * COLS * ROWS + col;

	/* trying to set a cell that's fixed */
	if (state!= edit && fixedBoard[index] != 0) {
		return -1;
	}
	if (board[index] == value) {
		return 0;
	}

	board[index] = value;

	return 0;
}

/* count_legal_options_cell: count the legal options for a cell.
 * @param int* board: the game board.
  * @param int x: row index.
 * @param int y: col index.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @return number of legal options at index x,y
 */

int count_legal_options_cell(int* board, int x, int y, int ROWS, int COLS){
	int BLOCKSIZE = ROWS*COLS;
	int option;
	int option_count= 0;
	for (option = 1; option < BLOCKSIZE + 1; option++) {
		/* if the option is valid add it */
		if(legal_value_in_position(board,x,y,option,ROWS,COLS)){
			option_count++;
		}
	}
	return option_count;
}

/* legal_value_in_position: returns 1 if the value is legal for the given position. 0 otherwise.
 * @param int* board: the game board.
 * @param int row: row index.
 * @param int col: col index.
 * @param int value: the value to check.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @return 1 if the value is legal for the given position. 0 otherwise
 */

int legal_value_in_position(int* board, int row, int col, int value, int ROWS, int COLS){
	int result;
	result =checkRow(board, row, col, value, ROWS, COLS)
							&& checkCol(board, row, col, value, ROWS, COLS)
							&& checkBlock(board, row, col, value, ROWS, COLS);
	return result;

}

/*clear_y_cells: used by Generate command- clears all but y random cells.
 * @param int* board: the game board.
 * @param int ROWS: dimensions of board.
 * @param int COLS: dimensions of board.
 * @param int Y_cells_to_keep: how many cells to keep in the board.
 * @return void
 */
void clear_y_cells(int* board, int ROWS, int COLS, int Y_cells_to_keep){
	int i,j;
	int x, y = 0;
	int boardSize = ROWS*COLS*ROWS*COLS;
	int* cells_to_keep = (int*) calloc(boardSize, sizeof(int));
	if (cells_to_keep == NULL ) {
				printFunctionError("calloc");
				exit(1);
			}
	while (Y_cells_to_keep > 0) {
		y = (rand()) % (ROWS * COLS);
		x = (rand()) % (ROWS * COLS);
		if(cells_to_keep[x*ROWS*COLS+y]==1){
			continue;
		}
		else{
		cells_to_keep[x*ROWS*COLS + y] = 1;
		Y_cells_to_keep--;
		}
	}
	for (i = 0; i < ROWS*COLS; ++i) {
		for (j = 0; j < ROWS*COLS; ++j) {
			if(cells_to_keep[i*ROWS*COLS+j]){
				continue;
			}
			else{
				board[i*ROWS*COLS+j] = 0;
			}
		}
	}
	free(cells_to_keep);


}

/*init_empty_board: initialize an empty 3x3 board- used by Edit command.
 * @param int** board: the game board.
 * @param int** fixed_board: the fixed_board.
 * @param int** error_board: the error_board.
 * @return void- changes in place the pointers to the board.
 */

void init_empty_board(int** board, int** fixed_board, int** error_board) {
	free(*board);
	free(*fixed_board);
	free(*error_board);

	*board = (int*) calloc(81, sizeof(int));
	*fixed_board = (int*) calloc(81, sizeof(int));
	*error_board = (int*) calloc(81, sizeof(int));

	if (*board == NULL || *fixed_board == NULL || *error_board==NULL ) {
		printFunctionError("calloc");
			exit(1);
		}
}
