
/* Game summary:
 * A module that is responsible for handling the flow of a game */

#include <stdlib.h>
#include <stdio.h>
#include "Solver.h"
#include "Parser.h"
#include "DoublyLinkedList.h"

#include "Game.h"
#include "main_aux.h"
#include "Error_Print.h"
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "Commands.h"
#include "Commands_aux.h"

/* The main function that plays a game
 * @params: None
 * @returns: -1 if problem occured- unable to continue, 0 if clean exit.*/
int play_game() {
	char* results = (char*) malloc(256); /* should be 252 */
	int* board = NULL;
	int* fixed_board = NULL;

	int* error_board = NULL;

	int* tmp_board = NULL;
	int* tmp_fixed_board = NULL;

	int ROWS = 0;
	int COLS = 0;
	int mark_errors = 1;
	int total_row_length;
	GAME_STATE state = init;
	int error_count = 0;
	int numCellsComplete = 0;

	int dimensions[2] = { 0 };

	NODE* redo_list_head = create_list();

	int command;

	if (results == NULL ) {
		printFunctionError("malloc");
		return -1;
	}
	while (1) {

		if(state==init){
			mark_errors=1;
		}
		results = (char*) realloc(results, 256);
		if (results == NULL ) {
			printFunctionError("realloc");
				return -1; }

		getCommand(state, results);

		command = results[0];
		/*		 */

		switch (command) {
		case -2: /*A memory error occured in Parser */
			return -1;
		case 0: /* command is exit */
			redo_list_head = delete_list(&redo_list_head);
			free(redo_list_head);
			free(results);
			free(tmp_board);
			free(tmp_fixed_board);
			free(board);
			free(fixed_board);
			free(error_board);
			return 0;
		case 2: /*command is edit */
			state = edit;
			if (results[1] == '\0') {
				init_empty_board(&board, &fixed_board, &error_board);
				ROWS = 3;
				COLS = 3;
				total_row_length = 9;

				redo_list_head = delete_list(&redo_list_head);

				printGame(board, fixed_board, error_board, ROWS, COLS,
						mark_errors, state);
				break;
			}

		case 1: /* command is solve */
			load_board(results + 1, &tmp_board, &tmp_fixed_board, dimensions);

			if (dimensions[0] == -1) {
				continue;
			}
			ROWS = dimensions[0];
			COLS = dimensions[1];
			total_row_length = ROWS * COLS;

			if (command == 1)
				state = solve;

			redo_list_head = delete_list(&redo_list_head);

			error_board = (int*) realloc(error_board,
					total_row_length * total_row_length * sizeof(int));
			board = (int*) realloc(board,
					total_row_length * total_row_length * sizeof(int));
			fixed_board = (int*) realloc(fixed_board,
					total_row_length * total_row_length * sizeof(int));

			if (board == NULL || fixed_board == NULL || error_board == NULL ) {
				printFunctionError("realloc");
				return -1;
			}

			copyBoard(tmp_board, board, total_row_length * total_row_length);
			copyBoard(tmp_fixed_board, fixed_board,
					total_row_length * total_row_length);

			error_count = count_errors_in_board(board, error_board, ROWS, COLS);


			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
					state);


			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if( check_winning( ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}
			break;


		case 3:
			mark_errors = results[1];
			printf("mark_errors has been switched to %d. Now errors will be %s \n", mark_errors, mark_errors? "shown.": "not shown.");
			break;
		case 4: /* command is print_board */
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
					state);
			break;
		case 5: /* command is set */
			set_command(&redo_list_head, board, fixed_board, results[1],
					results[2], results[3], ROWS, COLS,
					&error_count, error_board, state);


			error_count = count_errors_in_board(board,error_board,ROWS,COLS);
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
					state);

			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if( check_winning( ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}

			break;


		case 6: /*command is validate */
			validate(board, ROWS, COLS, error_count);
			break;
		case 7: /* command is guess */
			guess(board, &redo_list_head, ROWS, COLS, results+1, &error_count);

			error_count = count_errors_in_board(board,error_board,ROWS,COLS);
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors, state);

			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if( check_winning(ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}

			break;
		case 8: /* command is generate */
			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if ( generate(board, &redo_list_head, results[1], results[2], ROWS, COLS,
					error_count, numCellsComplete) ==1){
				error_count = count_errors_in_board(board,error_board,ROWS,COLS);
				numCellsComplete = results[2];
			}
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
								state);
			if( check_winning(ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}
			break;
		case 9: /* command is undo*/

			redo_list_head = move_back_or_forth(-1, redo_list_head, board,
					fixed_board, ROWS, COLS, state);
			error_count = count_errors_in_board(board, error_board, ROWS, COLS);

			error_count = count_errors_in_board(board, error_board, ROWS, COLS);
			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);

			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
					state);

			break;
		case 10: /*command is redo */
			redo_list_head = move_back_or_forth(1, redo_list_head, board,
					fixed_board, ROWS, COLS, state);

			error_count = count_errors_in_board(board, error_board, ROWS, COLS);

			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
								state);
			if( check_winning(ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}

			break;
		case 11: /* command is save */
			save_board(state, results + 1, board, fixed_board, ROWS, COLS,
					error_count);
			break;
		case 12: /* command is hint */
			hint(board, fixed_board, results[1], results[2], ROWS, COLS,
					error_count);
			break;
		case 13: /* command is guess_hint */
			guess_hint(board, fixed_board, results[1], results[2], ROWS, COLS,
					error_count);
			break;

		case 14: /* command is num_solutions */
			if(error_count>0){
				printf("The board contains errors, number of solutions is 0 \n");
				break;
			}
			num_solutions(board, ROWS, COLS);
			break;

		case 15: /* command is autofill */
			autofill(&redo_list_head, board,error_count, ROWS, COLS, 1,1);

			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors, state);
			error_count = count_errors_in_board(board,error_board,ROWS,COLS);
			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if( check_winning(ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}

			break;
		case 16: /* command is reset */
			while (redo_list_head->moves_count != -1) {
				redo_list_head = move_back_or_forth(-1, redo_list_head, board,
						fixed_board, ROWS, COLS, state);
			}
			error_count = count_errors_in_board(board,error_board,ROWS,COLS);
			printGame(board, fixed_board, error_board, ROWS, COLS, mark_errors,
					state);
			count_full_cells_in_board(board, ROWS, COLS, &numCellsComplete);
			if( check_winning(ROWS, COLS, numCellsComplete, error_count)){
				printWinMessage();
				state = init;
			}
			break;
		}

	}
	return 0;
}



/* * count_options: counts the legal options available for a single cell
 * @param: given_board : current game board
 * @param: i : the row index in question
 * @param: j : the  col in question
 * @param: ROWS COLS : dimensions of board
 * @returns: option count- the number of legal options for row i and column j*/

int count_options(int* given_board,int i, int j, int ROWS, int COLS){

	int option, option_num = 0;
	for(option= 0;option<ROWS*COLS;option++){
		if (checkRow(given_board, i,j, option, ROWS, COLS) == 1
				&& checkCol(given_board, i,j, option, ROWS, COLS) == 1
				&& checkBlock(given_board, i, j, option, ROWS, COLS) == 1) {
			option_num++;
		}
	}
	return option_num;
}
/*
 * check_winning: naive function that checks if all the cells are full and there are no errors- then the game is won
 * @param: ROWS COLS : dimensions of board
 * @param: numCellsComplete : number of cells with a value on the board
 * @param: error_count : number of errors in the board
 * @returns: 1 if the game is won, 0 if not*/
int check_winning(int ROWS, int COLS, int numCellsComplete, int error_count){
	if(numCellsComplete==ROWS*COLS*ROWS*COLS && error_count==0){
		return 1;
	}
	return 0;
}

/** count_full_cells_in_board: naive function to count all the full cells in the board and update numCellsComplete.
 * @param: board : current game board
 * @param: ROWS COLS : dimensions of board
 * @param: numCellsComplete* : a pointer to the variable holding the number of filled cells in the board

 * @returns: numCellsComplete - updating it in place and returning it as well*/
int count_full_cells_in_board(int* board, int ROWS, int COLS, int* numCellsComplete) {
	int result_full_cells= 0;
	int i, j;
	int total_row_length = ROWS * COLS;
	int index;
	int value;

	for (i = 0; i < total_row_length; i++) {
		for (j = 0; j < total_row_length; j++) {
			index = i * total_row_length + j;
			value = board[index];
			if(value){
				result_full_cells++;
			}
		}
	}
	*numCellsComplete = result_full_cells;
	return result_full_cells;

}


/* * count_errors_in_board: cound all the errors in the board, update the error_board.
 * @param: board : current game board
 * @param: error_board : the board holding 1's for errors in the board (for printing), 0 for correct cells (or empty cells)
 * @param: ROWS COLS : dimensions of board
 * @returns: number of errors in the board + updating the error_board in place*/
int count_errors_in_board(int* board, int* error_board, int ROWS, int COLS) {
	int result_num_errors = 0;
	int i, j;
	int total_row_length = ROWS * COLS;
	int index;
	int value;

	for (i = 0; i < total_row_length; i++) {
		for (j = 0; j < total_row_length; j++) {
			index = i * total_row_length + j;

			value = board[index];
			if(value && legal_value_in_position(board, i,j,value,ROWS,COLS)==0){
				result_num_errors++;
				error_board[index] = 1;
			} else {
				error_board[index] = 0;
			}
		}

	}
	return result_num_errors;
}






