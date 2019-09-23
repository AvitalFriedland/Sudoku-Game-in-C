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

#ifndef COMMANDS_H_
#define COMMANDS_H_

void load_board(char* path, int** board, int** fixed_board, int dimensions[]) ;

int set_command(NODE** current_list_ptr, int* board, int* fixed_board, int x,
		int y, int value, int ROWS, int COLS,
		int* error_count_ptr, int* error_board,GAME_STATE state) ;


void validate(int* board, int ROWS, int COLS, int numErrors) ;


void guess(int* board, NODE** current_list_ptr, int ROWS,
		int COLS, char* threshold_ptr_chars, int* error_count_ptr) ;
/* returns 0 if failed to execute the command, board is unchanged */
int generate(int* board, NODE** current_list_ptr, int X_rand_cells, int Y_cells_left,
		int ROWS, int COLS, int error_count, int numCellsComplete);


void save_board(GAME_STATE state, char* path, int* board, int* fixed_board,
		int ROWS, int COLS, int error_count);

void hint(int* board, int* fixed_board, int x, int y, int ROWS, int COLS,
		int numErrors);

void guess_hint(int* board, int* fixed_board, int x, int y, int ROWS, int COLS,
		int numErrors);

int autofill(NODE** current_list_ptr, int* board,
		int error_count, int ROWS, int COLS,
		int ADD_TO_REDO_LIST, int FILL_ERRORS);



#endif /* COMMANDS_H_ */
