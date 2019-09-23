/*
 * Commands module summary
 * includes all the main commands available to the user and their implemantation. gets the input mainly from play_game in Game.c module.
 *  load_board - called by using "solve file" or "edit file", loads the board from the file.
 * set_command - called by using "set x y z". performs the set command.
 *validate - uses ILP to validate the board. as required.
 *guess - the command as required
 *generate - the command as required
 *save_board - the command as required
 *hint - the command as required
 *guess_hint - the command as required
 *autofill - the command as required. if ADD_TO_REDO_LIST is 1 then the moves are added to the linked list. if FILL_ERRORS is 1 it is ok to fill errors.
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "Error_Print.h"
#include "Commands_aux.h"
#include "DoublyLinkedList.h"
#include "Gurobi_Code.h"
#include "main_aux.h"
#include "Commands.h"
#include "Parser_aux.h"
#include "Game.h"

/*
 *  load_board - called by using "solve file" or "edit file", loads the board from the file.
 *  @param: path - the path to the file.
 *  @param: board - a pointer to the board object- to update it.
 *  @param: fixed_board - a pointer to the fixed_board object.
 *  @param: dimensions - an array of dimensions, used to pass information back to play_game
 *  @return void.
 */
void load_board(char* path, int** board, int** fixed_board, int dimensions[]) {

	FILE* file = fopen(path, "r");
	int result_of_scanf;
	int ROWS;
	int COLS;

	int totalRowLength;

	int current_char;

	int three_last_chars[3] = { 0 };
	int i = 0;

	int row_index = 1;
	int col_index = 1;

	int board_index = 0;
	int count_num_values = 0;
	int num_wanted_values = 0;

	if (file == NULL ) {
		printFileOpenError(path);
		dimensions[0] = -1;
		return;
	}
	result_of_scanf = fscanf(file, "%d %d", &dimensions[0], &dimensions[1]);


	if (result_of_scanf != 2) {
		printErrorFileFormat("the dimensions were not specified \n");
		dimensions[0] = -1;
		fclose(file);
		return;
	}
	if (!checkDimensions(dimensions)) {
		printErrorFileDimensions();
		dimensions[0] = -1;
		fclose(file);
		return;
	}
	ROWS = dimensions[0];
	COLS = dimensions[1];
	totalRowLength = ROWS * COLS;
	num_wanted_values = totalRowLength*totalRowLength;

	free(*board);
	free(*fixed_board);
	*board= (int*) malloc(totalRowLength * totalRowLength * sizeof(int));
	*fixed_board= (int*) malloc(totalRowLength * totalRowLength * sizeof(int));
	if(*board==NULL || *fixed_board==NULL){
		printFunctionError("malloc");
	}
	/*
	 [1,2,3] - bad
	 [1,2,.] - ok
	 [1,., __] - ok
	 */
	while ((current_char = fgetc(file)) != EOF) {
		if(!isspace(current_char) && count_num_values==num_wanted_values ){
			printErrorFileFormat("it included too many values for a board with these dimensions\n");
			dimensions[0] = -1;
			fclose(file);
			return;

		}

		if (current_char == '.') {
			if (i == 0) {
				printErrorFileFormat("a dot was found without a digit before it. \n ");
				dimensions[0] = -1;
				fclose(file);

				return;
			}
			current_char = fgetc(file); /*read the next char after . */
			board_load_value(*board, *fixed_board, board_index, three_last_chars, i, 1); /* flag ==1 because we write to fixed_board too */

			count_num_values++;
			if (increase_indices(&board_index, &col_index, &row_index,
					totalRowLength) == -1) {
				break;
			}
			if (isspace(current_char)) {
				i = 0;
				continue;
			}
			if (current_char == EOF) {

				break;
			}
			printErrorFileFormat("a character which is not a space was encounterd after a dot, it is invalid. \n");

			dimensions[0] = -1;
			fclose(file);

			return;
		}

		if (isspace(current_char)) {
			if (i == 0) {
				continue;
			}

			board_load_value(*board, *fixed_board, board_index,
					three_last_chars, i, 0); /*flag ==0 because we dont write to fixed_board too */
			count_num_values++;

			i = 0;
			if (increase_indices(&board_index, &col_index, &row_index,
					totalRowLength) == -1) {
				break;
			}
			continue;
		}

		if (isdigit(current_char)) {
			if (i == 2) {
				printErrorFileFormat("a number with 3 digits was read, it is invalid. \n");
				dimensions[0] = -1;
				fclose(file);

				return;
			} else {
				three_last_chars[i] = current_char;
				i++;
			}
		} else {
			printErrorFileFormat("a character which is neither a digit nor a dot was found, it is invalid.\n");
			dimensions[0] = -1;
			fclose(file);
			return;

		}
	}
	if (i > 0) {
		board_load_value(*board, *fixed_board, board_index, three_last_chars, i,
				0);
		count_num_values++;
	}

	if(count_num_values<num_wanted_values ){
		printErrorFileFormat("it included too few values for a board with these dimensions\n");
		dimensions[0] = -1;
	}
	fclose(file);
	return;

}

/*
 * set_command - called by using "set x y z". performs the set command.
 *  @param: current_list_ptr - a pointer to the head of the linked list.
 *  @param: board - a pointer to the board object- to update it.
 *  @param: fixed_board - a pointer to the fixed_board object.
 *  @params int x,int y, int value - index to insert + value to insert respectively.
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param int* error_count_ptr - number of errors in the board.
 *  @param int* error_board - the error board.
 *  @param GAME_STATE state - the state of the game.
 *
 *  @return 1 if successful, -1 otherwise.
 */
int set_command(NODE** current_list_ptr, int* board, int* fixed_board, int x,int y, int value, int ROWS, int COLS,int* error_count_ptr, int* error_board, GAME_STATE state) {

	int result;
	MOVE* set_move = (MOVE*) malloc(sizeof(MOVE));

	MOVE* moves = (MOVE*) malloc(sizeof(MOVE));

	if(set_move==NULL || moves== NULL){
		printFunctionError("malloc");
		exit(1);
	}

	set_move->x = x;
	set_move->y = y;
	set_move->after = value;
	set_move->before = board[(y - 1) * ROWS * COLS + (x - 1)];

	moves[0] = *set_move;

	x--;
	y--;
	if (x < 0 || y < 0){
		printInvalidParameterRangeInt(5, x<0? x+1: y+1, "1-size of block");
		free(set_move);
		free(moves);

		return -1;
	}
	if (x+1 > ROWS*COLS) {
		printInvalidParameterRangeInt(5, x+1, "1-size of block");
		free(set_move);
		free(moves);

		return -1;
	}
	if (y+1 > ROWS*COLS) {
		printInvalidParameterRangeInt(5, y+1, "1-size of block");
		free(set_move);
		free(moves);

		return -1;
	}
	if (value>ROWS*COLS) {
		printInvalidParameterRangeInt(5, value, "between 1 and the block size");
		free(set_move);
		free(moves);

		return -1;
	}

	result = set(board, fixed_board, x, y, value, ROWS, COLS, state);

	if (result == -1) {
		printCellIs(x + 1, y + 1, "fixed and so cannot be set.");
		free(set_move);
		free(moves);
		return -1;
	}

	*error_count_ptr = count_errors_in_board(board, error_board, ROWS, COLS);

	(*current_list_ptr) = add_after((*current_list_ptr), moves, 1);
	free(set_move);

	return 1;
}

/*
 *  validate - uses ILP to validate the board. as required.
 *  @param: board - a pointer to the board object- to update it.
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param: num errors - the number of errors in the board
 *   @return void.
 */
void validate(int* board, int ROWS, int COLS, int numErrors) {
	int result;
	int* copy_board;
	int totalRowLength=  ROWS*COLS;
	int autofill_changes = 0;

	if (numErrors != 0) {
		printBoardErroneous(6);
		return;
	}
	copy_board = (int*) malloc(totalRowLength * totalRowLength * sizeof(int));

	copyBoard(board, copy_board, totalRowLength*totalRowLength);

	/* exahust autofill to reduce constraints in Gorubi */
	autofill_changes = autofill(NULL, copy_board, numErrors, ROWS, COLS, 0, 0);
	while (  autofill_changes!=0){
		autofill_changes = autofill(NULL, copy_board, numErrors, ROWS, COLS, 0, 0);
	}
	/* */
	result = ILP(copy_board, NULL, ROWS, COLS);
	switch (result) {
	case 0:
		printf("Board is solvable\n");
		break;
	case -1:
		printf("Board is unsolvable\n");
		break;
	case -2:
		printf("Gurobi has failed\n");
	}
	free(copy_board);
}
/*
 *	guess - the command as required
 *  @param: board - a pointer to the board object- to update it.
 *  @param: current_list_ptr - a pointer to the head of the linked list.
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param int* error_count_ptr - pointer to number of errors in the board.
 *  @param int* threshold_ptr_chars - the threshold for the command.
 *  @return void.
 */

void guess(int* board, NODE** current_list_ptr, int ROWS,
		int COLS, char* threshold_ptr_chars, int* error_count_ptr) {
	double* solution_board;
	int* board_copy;

	int index, i, j, val, result;
	int totalRowLength = ROWS * COLS;
	double maxFloat, randFloat;


	double threshold = parseFloatFromCharArray(threshold_ptr_chars);

	int move_count = 0;

	if (*error_count_ptr) {
		printBoardErroneous(13);
		return;
	}

	board_copy = (int*) calloc(totalRowLength * totalRowLength,	sizeof(int));
	if (board_copy == NULL ) {
		printFunctionError("calloc");
		return;
	}
	copyBoard(board, board_copy, totalRowLength * totalRowLength);

	solution_board = (double*) malloc(totalRowLength * totalRowLength * totalRowLength * sizeof(double));
	if(solution_board==NULL){
		printFunctionError("malloc");
		free(board_copy);
		return;
	}

	result = LP(board, solution_board, ROWS, COLS);
	result = 0;
	if (result == -1) {
		printf("Could not execute Guess command because the board is not solvable.\n");
	}
	if (result == -2) {
		printf("Gurobi has failed\n");
	}
	if (result == 0) {

		for (index = 0; index < totalRowLength * totalRowLength; index++) {
			i = index / totalRowLength;
			j = index - (index / totalRowLength) * totalRowLength;

			if (board[index] != 0) {
				continue;
			}
			maxFloat = 0;
			/* calculate the sum of all scores for a cell */
			for (val = 0; val < totalRowLength; val++) {
				if (solution_board[index * totalRowLength + val] >= threshold && legal_value_in_position(board,i,j,val+1,ROWS,COLS)==1) {
					maxFloat += solution_board[index * totalRowLength + val];
				}
			}
			/* get a random float number and choose with it the relevant value with relevant score */
			if (maxFloat > 0) {
				randFloat = (float) rand() / (float) RAND_MAX * maxFloat;
				for (val = 0; val < totalRowLength; val++) {
					if (solution_board[index * totalRowLength + val] >= threshold && legal_value_in_position(board,i,j,val+1,ROWS,COLS)==1) {
						randFloat -= solution_board[index * totalRowLength + val];
					}
					/* if randFloat is below zero it means we just got to the relevant value */
					if (randFloat <= 0) {
						board[index] = val + 1;
						move_count++;
						break;
					}
				}
			}
		}
		if (move_count != 0) {
			add_to_redo_list(board_copy, board, current_list_ptr, ROWS,COLS,move_count);
		}
	}
	free(board_copy);
	free(solution_board);

}

/*
 *	generate - the command as required
 *  @param: board - a pointer to the board object- to update it.
 *  @param: current_list_ptr - a pointer to the head of the linked list.
 *  @param: int X_rand_cells- parameter how many random cells to fill
 *  @param: int Y_cells_left - parameter how many random cells to keep
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param int error_count -  number of errors in the board.
 *  @param int numCellsComplete -  number of cells full in the board.
*  	@return 0 if failed to execute the command, board is unchanged */
int generate(int* board, NODE** current_list_ptr, int X_rand_cells, int Y_cells_left,
		int ROWS, int COLS, int error_count, int numCellsComplete) {
	int x, y = 0;
	int value;
	int totalRowLength = ROWS*COLS;
	int NUM_ITERATIONS=1000;
	int* solution_board;
	int* copy_board;
	int result_of_ilp = -2;
	int move_count = 0;
	int X_cells_left = X_rand_cells;
	int autofill_changes= 0;


	if (error_count > 0) {
		printBoardErroneous(8);
		return 0;
	}
	if (numCellsComplete+X_rand_cells > totalRowLength*totalRowLength){
		printBoardIsTooFull(X_rand_cells, totalRowLength*totalRowLength-numCellsComplete);
		return 0;
	}

	copy_board = (int*) malloc(
			totalRowLength * totalRowLength * sizeof(int));
	if(copy_board==NULL){
		printFunctionError("malloc");
		exit(1);
	}
	copyBoard(board, copy_board, totalRowLength *totalRowLength);

	/* fill X empty cells randomly */
	while(NUM_ITERATIONS>0){

		while (X_cells_left > 0) {
			y = (rand()) % (ROWS * COLS);
			x = (rand()) % (ROWS * COLS);

			if (copy_board[x * COLS * ROWS + y] == 0) {
				value = (rand()) % (ROWS * COLS);
				if( count_legal_options_cell(copy_board, x,y,ROWS,COLS)==0){
					copyBoard(board, copy_board, totalRowLength *totalRowLength);
					NUM_ITERATIONS--;
					X_cells_left  = X_rand_cells;
					continue;
				}
				while(legal_value_in_position(copy_board, x, y, value, ROWS, COLS)==0){
					value = 1+(rand()) % (ROWS * COLS);
				}
				copy_board[x * COLS * ROWS + y] = value;

				X_cells_left--;
			}
		}

		/* solve via ILP */
		solution_board = (int*) malloc(
				totalRowLength * totalRowLength * sizeof(int));

		if(solution_board==NULL){
			printFunctionError("malloc");
			free(copy_board);
			exit(1);
		}



		/* exahust autofill to reduce constraints in Gorubi */
		autofill_changes = autofill(current_list_ptr, copy_board, error_count, ROWS, COLS, 0,0);
		while (  autofill_changes!=0){
			autofill_changes = autofill(current_list_ptr, copy_board, error_count, ROWS, COLS, 0,0);
		}
		copyBoard(copy_board, solution_board, totalRowLength *totalRowLength);

		result_of_ilp = ILP(copy_board, solution_board, ROWS, COLS);

		if (result_of_ilp==-1) {
			NUM_ITERATIONS--;
			copyBoard(board, copy_board, totalRowLength *totalRowLength);

		}
		else if(result_of_ilp ==0){
			clear_y_cells(solution_board, ROWS, COLS, Y_cells_left);

			add_to_redo_list(board, solution_board,current_list_ptr, ROWS,COLS, move_count);


			copyBoard(solution_board, board, totalRowLength *totalRowLength);

			free(solution_board);
			free(copy_board);

			return 1;

		}
		else{
			free(solution_board);
			free(copy_board);

		}

	}
	printf("could not generate board : exhausted 1000 tries\n");
	return 0;


}

/*
 *	save_board -save- the command as required
 *	@param: state- the board state.
 *  @param: path - the path to the file.
 *  @param: board - a pointer to the board object- to update it.
 *  @param: fixed_board - a pointer to the board object- to update it.
 *  @param: ROWS, COLS - dimensions of board
 *  @param: error_count-  number of errors in the board.
 *  @return void. prints on success\ failure.
*/
void save_board(GAME_STATE state, char* path, int* board, int* fixed_board,
		int ROWS, int COLS, int error_count) {
	int result_validation, i, j;
	FILE* file;
	int* copy_board;
	int autofill_changes = 0;
	int totalRowLength = ROWS*COLS;
	if (state == edit) {
		if (error_count > 0) {
			printErrorSave("the board contains errors. fix them first to save \n");
			return;
		}
		copy_board = (int*) malloc(totalRowLength * totalRowLength * sizeof(int));
		if (copy_board == NULL ) {
			printFunctionError("malloc");
			exit(1);
		}
		copyBoard(board, copy_board, totalRowLength*totalRowLength);

		/* exahust autofill to reduce constraints in Gorubi */
		autofill_changes = autofill(NULL, copy_board, error_count, ROWS, COLS, 0,0);
		while (  autofill_changes!=0){
			autofill_changes = autofill(NULL, copy_board, error_count, ROWS, COLS, 0,0);
		}
		result_validation  = ILP(copy_board, NULL, ROWS, COLS);
		if (result_validation == -1) {
			printErrorSave("the board is unsolvable, so it cannot be saved. \n");
			free(copy_board);
			return;
		}
		if (result_validation == -2) {
			printErrorSave("validation of the board failed (Gurobi failed). try again \n");
			free(copy_board);
			return;
		}
	}
	file = fopen(path, "w");
	if (file == NULL ) {
		printFileOpenError(path);
		return;
	}

	fprintf(file, "%d %d \n", ROWS, COLS);
	for (i = 0; i < (ROWS * COLS); i++) {
		for (j = 0; j < COLS * ROWS; j++) {
			/* formula for accessing board[i][j] */
			if (state==edit || fixed_board[i * ROWS * COLS + j] != 0) {
				fprintf(file, " %d. ", board[i * ROWS * COLS + j]);
			} else {
				fprintf(file, " %d ", board[i * ROWS * COLS + j]);
			}
		}
		fprintf(file, "\n");
	}

	fclose(file);
	printf("board saved successfully to %s\n", path);
}
/*
 *	hint - the command as required
 *  @param: board - a pointer to the board object- to update it.
 *  @param: fixed_board - a pointer to the board object- to update it.
 *  @params x y - indices to give hint.
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param: numErrors-  number of errors in the board.
 *  return void */

void hint(int* board, int* fixed_board, int x, int y, int ROWS, int COLS,
		int numErrors) {
	int* solution_board;
	int* copy_board;
	int result, totalRowLength = ROWS * COLS, index = (y - 1) * totalRowLength
			+ x - 1;
	int autofill_changes = 0;

	if (x > totalRowLength || y > totalRowLength) {
		printInvalidParameterRangeInt(12, x > y ? x : y, "1-size of block");
		return;
	}
	if (x <= 0 || y <= 0) {
		printInvalidParameterRangeInt(12, x < y ? x : y, "1-size of block");
		return;
	}
	if (numErrors) {
		printBoardErroneous(12);
		return;
	}
	if (fixed_board[index]) {
		printCellIs(x, y, "fixed, and so a hint for it cannot be given.");
		return;
	}
	if (board[index]) {
		printCellIs(x, y,
				"already full, and so a hint for it cannot be given.");
		return;
	}
	solution_board = (int*) malloc( totalRowLength * totalRowLength * sizeof(int));
	copy_board = (int*) malloc(totalRowLength * totalRowLength * sizeof(int));
	copyBoard(board, copy_board, totalRowLength*totalRowLength);

	/* exahust autofill to reduce constraints in Gorubi */
	autofill_changes = autofill(NULL, copy_board, numErrors, ROWS, COLS, 0,0);
	while (  autofill_changes!=0){
		autofill_changes = autofill(NULL, copy_board, numErrors, ROWS, COLS, 0,0);
	}

	result = ILP(copy_board, solution_board, ROWS, COLS);
	if (result == -1) {
		printf("Could not give hint because the board is not solvable.\n");
	}
	if (result == -2) {
		printf("Gurobi has failed\n");
	}
	if (result == 0) {
		printf("cell <%d,%d> should be %d\n", x, y, solution_board[index]);
	}
	free(solution_board);
	free(copy_board);
}

/*
 *	guess_hint - the command as required
 *  @param: board - a pointer to the board object- to update it.
 *  @param: fixed_board - a pointer to the board object- to update it.
 *  @params x y - indices to give hint.
 *  @params int ROWS, int COLS - dimensions of the board.
 *  @param: numErrors-  number of errors in the board.
 *  return void */

void guess_hint(int* board, int* fixed_board, int x, int y, int ROWS, int COLS,
		int numErrors) {
	double* solution_board;
	int* copy_board;
	int i, result, totalRowLength = ROWS * COLS, index = (y - 1)
									* totalRowLength + x - 1;
	if (x > totalRowLength || y > totalRowLength) {
		printInvalidParameterRangeInt(12, x > y ? x : y, "1-size of block");
		return;
	}
	if (x <= 0 || y <= 0) {
		printInvalidParameterRangeInt(12, x < y ? x : y, "1-size of block");
		return;
	}
	if (numErrors) {
		printBoardErroneous(13);
		return;
	}
	if (fixed_board[index]) {
		printCellIs(x, y, "fixed, and so a hint for it cannot be given.");
		return;
	}
	if (board[index]) {
		printCellIs(x, y,
				"already full, and so a hint for it cannot be given.");
		return;
	}


	solution_board = (double*) malloc(totalRowLength * totalRowLength * totalRowLength * sizeof(double));
	copy_board = (int*) malloc(totalRowLength * totalRowLength * sizeof(int));

	copyBoard(board, copy_board, totalRowLength*totalRowLength);

	result = LP(copy_board, solution_board, ROWS, COLS);
	if (result == -1) {
		printf("Could not give hint because the board is not solvable.\n");
	}
	if (result == -2) {
		printf("Gurobi has failed\n");
	}
	if (result == 0) {
		printf("cell <%d,%d> scores for values are:\n", x, y);
		for (i = 0; i < totalRowLength; i++) {
			if (solution_board[index * totalRowLength + i]) {
				printf("[%d] score: %f\n", i + 1,
						solution_board[index * totalRowLength + i]);
			}
		}
	}
	free(solution_board);
	free(copy_board);
}
/*
 *	autofill - the command as required
 *  @param: current_list_ptr - a pointer to the head of the linked list.
 *  @param: board - a pointer to the board object- to update it.
 *  @param: error_count-  number of errors in the board.
 *  @params int ROWS, int COLS - dimensions of the board.

 *  @param: ADD_TO_REDO_LIST - a flag- if it is 1 then the autofill moves are added to the linekd list
 *  @params FILL_ERRORS- a flag- if it is 1 then autofill will make errors, it not - it won't.
 *  return the number of moves made by the command
 *  */

int autofill(NODE** current_list_ptr, int* board,
		int error_count, int ROWS, int COLS,
		int ADD_TO_REDO_LIST, int FILL_ERRORS) {
	int total_row_length = ROWS * COLS;
	int* board_copy;
	int i, j, option;
	int valid_option;
	int option_count;


	int move_count = 0;

	if (error_count > 0) {
		printBoardErroneous(15);
		return 0;
	}
	board_copy = (int*) calloc(total_row_length * total_row_length,
			sizeof(int));

	if (board_copy == NULL ) {
		printFunctionError("calloc");
		return 0;
	}
	copyBoard(board, board_copy, total_row_length * total_row_length);
	for (i = 0; i < total_row_length; i++) {
		for (j = 0; j < total_row_length; j++) {
			if (board_copy[i * total_row_length + j] != 0)
				continue;
			option_count = 0;
			for (option = 1; option < total_row_length + 1; option++) {
				/* if the option is valid add it */
				if(legal_value_in_position(board_copy,i,j,option,ROWS,COLS)==1){
					valid_option = option;
					option_count++;
				}
			}
			if (option_count == 1 && board[i*ROWS*COLS+j] ==0){
				if(FILL_ERRORS || legal_value_in_position(board,i,j,option,ROWS,COLS)==1){
					board[i*ROWS*COLS +j ] = valid_option;
					move_count++;
				}
			}

		}
	}
	if (move_count == 0) {
		free(board_copy);
		return 0;
	}

	if (ADD_TO_REDO_LIST){ add_to_redo_list(board_copy, board, current_list_ptr, ROWS, COLS, move_count);
	}
	free(board_copy);
	return move_count;
}





