/* Solver summary:
 * A module that is responsible for solving a sudoku board in a "classic way" - no LP.
 * */
#ifndef SOLVER_H_
#define SOLVER_H_

/* num_solutions: a function that counts the number of possible solutions to the current board (if any), iteratively, using a stack implementation */
void num_solutions(int* board, int ROWS, int COLS);
#endif /* SOLVER_H_ */
