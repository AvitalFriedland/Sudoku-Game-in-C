/* main_aux summary:
 * A module that is responsible for some auxillary functions
 *
 * copyBoard: copies "boardOrig" INTO "boardDest"
 * copyFloatBoard: copies "boardOrig" INTO "boardDest". this is exactly like copyBoard only it deals with DOUBLES instead of INTS.
 * checkBlock: checks if it's possible to insert OPTION in then given BLOCK
 * checkRow checks if it's possible to insert OPTION in then given ROW
 * checkCol checks if it's possible to insert OPTION in then given COLUMN
 * printGame: given the board and the fixed cells board, prints the game according to our instructions
 * printRowDivider: helper function to print the correct number of "-".
 * */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_
#include "Game.h"


void printGame(int board[], int fixed_board[], int error_board[], int ROWS, int COLS, int mark_errors, GAME_STATE state);
void printRowDivider();
int getFixedCells();
void printErrorFunc(char* funcName);
void copyBoard(int* boardOrig, int* boardDest, int length);
void copyFloatBoard(double* boardOrig, double* boardDest, int length);
int checkBlock(int solved[], int i, int j, int option, int ROWS, int COLS);
int checkRow(int solved[], int i,int j, int option, int ROWS, int COLS);
int checkCol(int solved[], int i,int j, int option, int ROWS, int COLS);


#endif /* MAIN_AUX_H_ */
