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

#ifndef COMMANDS_AUX_H_
#define COMMANDS_AUX_H_
#include "DoublyLinkedList.h"
#include "Game.h"
void board_load_value(int* board, int* fixed_board, int board_index,int* three_last_chars, int i, int flag_fixed);

void undo_redo_moves(int undo_redo, MOVE* moves_list, int num_moves,int* board, int* fixed_board, int ROWS, int COLS, GAME_STATE state);

int checkDimensions(int* dimensions);

int increase_indices(int* board_index, int* col_index, int* row_index,
		int totalRowLength);

void add_to_redo_list(int* original_board, int* board_after_changes,NODE** current_list_ptr, int ROWS, int COLS, int move_count);

int set(int* board, int* fixedBoard, int x, int y, int value, int ROWS, int COLS, GAME_STATE state);

int count_legal_options_cell(int* board, int x, int y, int ROWS, int COLS);

int legal_value_in_position(int* board, int row, int col, int value, int ROWS, int COLS);

void clear_y_cells(int* board, int ROWS, int COLS, int Y_cells_to_keep);

void init_empty_board(int** board, int** fixed_board, int** error_board) ;

#endif /* COMMANDS_AUX_H_ */
