/* Game summary:
 * A module that is responsible for handling the flow of a game
 *
 * play_game: The main function that plays a game
 * check_winning: naive function that checks if all the cells are full and there are no errors- then the game is won
 * count_options: counts the legal options available for a single cell
 * count_errors_in_board: cound all the errors in the board, update the error_board.
 * count_full_cells_in_board: naive function to count all the full cells in the board and update numCellsComplete.
 * */
#ifndef GAME_H_
#define GAME_H_




typedef enum game_state {
	init = 0, edit = 1, solve = 2
} GAME_STATE;

int check_winning(int ROWS, int COLS, int numCellsComplete, int error_count);
int play_game();
int count_options(int* given_board,int i, int j, int ROWS, int COLS);
int count_full_cells_in_board(int* board, int ROWS, int COLS, int* numCellsComplete) ;
int count_errors_in_board(int* board, int* error_board, int ROWS, int COLS);


#endif /* GAME_H_ */
