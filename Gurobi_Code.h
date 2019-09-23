/*
 * Gorubi_Code module summary:
 * 	ILP:
 * 	solves the board with Ineteger Linear Programming. the objective function is maximise 0 (nonexistant) and the goal of the Linear Programming is to find a feasible solution.
 * 	The variable types are BINARY and so only 1's and 0's.
 *
 * 	LP:
 * 	solves the board with regular Linear Programming. the variables aren't limited to Binary and so continous values are returned.
 * 	We compute the objective function in the following way:
 * 		for a cell with N possible values, we calculate it's DF = degrees of freedom with the following equation:
 * 		1 /  (number of empty cells in the cells' row) + 1 /  (number of empty cells in the cells' column) + 1 /  (number of empty cells in the cells' block)
 * 		To this number, a small random number is added to add some variability and improve the function on average.
 * 		This coefficient is assigned to each of the N possible values (with a small random factor).
 * 	Our model uses maximization, the reason for that follows logically from the way the coefficients are calculated:
 * 	 	for a cell with a lot of degrees of freedom, it is given a small coefficient - because the model shouldn't be very "sure" of its value.
 * 	 	for a cell with small degrees of freedom, it is given a bigger coefficient.
 */

#ifndef GORUBI_CODE_H_
#define GORUBI_CODE_H_




int ILP(int* board, int* solution_board, int ROWS, int COLS);

int LP(int* board, double* solution_board, int ROWS, int COLS);

#endif /* GORUBI_CODE_H_ */
