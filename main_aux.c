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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_aux.h"
#include "Error_Print.h"
#include "Game.h"

/* copies "boardOrig" INTO "boardDest"
 * params: original board to copy into destination board + length of total board
 * returns: void, change in place*/
void copyBoard(int* boardOrig, int* boardDest, int length) {
	int i = 0;
	for (i = 0; i < length; ++i) {
		boardDest[i] = boardOrig[i];
	}
}
/* copies "boardOrig" INTO "boardDest". this is exactly like copyBoard only it deals with DOUBLES instead of INTS.
 * params: original board to copy into destination board + length of total board
 * returns: void, change in place*/

void copyFloatBoard(double* boardOrig, double* boardDest, int length) {
	int i = 0;
	for (i = 0; i < length; ++i) {
		boardDest[i] = boardOrig[i];
	}
}

/* function to print the board
 * params: the board, the fixed board (to add dots if necessary),the error_board (to add asterisks if necessary), rows cols number
 * mark_errors to decide if to mark the errors, and the state of the board.
 * returns: void */
void printGame(int board[], int fixed_board[], int error_board[], int ROWS, int COLS, int mark_errors, GAME_STATE state) {
	int i,j;
	int tmpi, tmpj;
	for (i = 0; i < ((ROWS + 1) * COLS + 1); i++) {
		if (i % (ROWS + 1) == 0)
			printRowDivider(ROWS, COLS);
		else {
			for (j = 0; j < (COLS + 1) * ROWS; j++) {
				if (j % (COLS + 1) == 0) {
					printf("|");
				} else {
					printf(" ");
					/* formula for accessing board[i][j] */
					tmpi = i - (i / (ROWS + 1)) - 1;
					tmpj = j - (j / (COLS + 1)) - 1;
					if (state!=edit && fixed_board[tmpi * ROWS * COLS + tmpj] != 0) {
						printf("%2d.", board[tmpi * ROWS * COLS + tmpj]);
					}
					else if (board[tmpi * ROWS * COLS + tmpj] == 0) {
						printf("   ");
					}
					else if ((state==edit || mark_errors) && error_board[tmpi * ROWS * COLS + tmpj] != 0) {
						printf("%2d*", board[tmpi * ROWS * COLS + tmpj]);
					}
					 else {
						printf("%2d ", board[tmpi * ROWS * COLS + tmpj]);
					}
				}
			}
			printf("|\n");
		}
	}
}


/* prints the row divider */
void printRowDivider(int ROWS, int COLS) {
	int i=0;
	for(i=0;i< 4*ROWS*COLS+ROWS+1; i++){
		printf("-");
	}
	printf("\n");
}


/* parameters: the current board to check, indices i and j to check, option to check
 * returns: checks if it's possible to insert OPTION in then given BLOCK
 * returns 0 if not possible, 1 if is
 */
int checkBlock(int solvedBoard[], int i, int j, int option, int ROWS, int COLS) {
	int blockRow = i / ROWS;
	int blockCol = j / COLS;
	int irows;
	int jcols;
	for (irows = blockRow * ROWS; irows < (blockRow + 1) * ROWS; irows++) {
		for (jcols = blockCol * COLS; jcols < (blockCol + 1) * COLS; jcols++) {
			if ((irows!=i || jcols!=j )&& solvedBoard[irows * COLS * ROWS + jcols] == option) {
				return 0;
			}
		}
	}
	return 1;
}


/* parameters: the current board to check, index i to check, option to check
 * checks if it's possible to insert OPTION in then given ROW
 * returns 0 if not possible, 1 if is */
int checkRow(int solvedBoard[], int ROW_NUM, int COL_NUM, int option, int ROWS, int COLS) {
	int k;
	for (k = 0; k < ROWS * COLS; k++) {
		if (k!=COL_NUM &&solvedBoard[ROW_NUM* ROWS * COLS + k] == option) {
			return 0;
		}
	}
	return 1;
}

/* parameters: the current board to check, index j to check, option to check
 * checks if it's possible to insert OPTION in then given COLUMN
 * returns 0 if not possible, 1 if is */
int checkCol(int solvedBoard[],int ROW_NUM, int COL_NUM, int option, int ROWS, int COLS) {
	int k;
	for (k = 0; k < ROWS * COLS; k++) {
		if (k!= ROW_NUM && solvedBoard[k * ROWS * COLS + COL_NUM] == option) {
			return 0;
		}
	}
	return 1;
}



