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

#include <time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "Solver.h"
#include "main_aux.h"
#include "gurobi_c.h"



/*
 * @param given_board: the current board
 * @param solution_board : the solution board to return
 * @param ROWS: board dimensions
 * @param COLS board dimensions
 */
int ILP(int* given_board, int* solution_board, int ROWS, int COLS)
{
	int DIM = ROWS*COLS;
	int status = 0;
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;

	int* 		board = (int*) malloc(DIM*DIM*sizeof(int));
	int*       ind = (int*) malloc(DIM*sizeof(int));
	double*     val = (double*) malloc(DIM*sizeof(double));

	double*     lb = (double*) malloc(DIM*DIM*DIM*sizeof(double));
	char*     vtype = (char*) malloc(DIM*DIM*DIM);
	double*     sol = (double*) malloc(DIM*DIM*DIM*sizeof(double));

	int       optimstatus;
	double    objval;
	int       i, j, v, ig, jg, count;
	int       error = 0;


	copyBoard(given_board, board, DIM*DIM);
	for (i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			board[i*DIM + j] -= 1;
		}
	}

	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				if (board[i*DIM+ j] == v)
					lb[i*DIM*DIM+j*DIM+v] = 1;
				else
					lb[i*DIM*DIM+j*DIM+v] = 0;
				vtype[i*DIM*DIM+j*DIM+v] = GRB_BINARY;
			}
		}
	}

	/* Create environment */

	error = GRBloadenv(&env, "sudoku.log");
	if (error) goto QUIT;

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) goto QUIT;
	/* Create new model */

	error = GRBnewmodel(env, &model, "sudoku", DIM*DIM*DIM, NULL, lb, NULL,
			vtype, NULL);
	if (error) goto QUIT;

	/* Each cell gets a value */

	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				ind[v] = i*DIM*DIM + j*DIM + v;
				val[v] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}

	/* Each value must appear once in each row */

	for (v = 0; v < DIM; v++) {
		for (j = 0; j < DIM; j++) {
			for (i = 0; i < DIM; i++) {
				ind[i] = i*DIM*DIM + j*DIM + v;
				val[i] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}

	/* Each value must appear once in each column */

	for (v = 0; v < DIM; v++) {
		for (i = 0; i < DIM; i++) {
			for (j = 0; j < DIM; j++) {
				ind[j] = i*DIM*DIM + j*DIM + v;
				val[j] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) goto QUIT;
		}
	}

	/* Each value must appear once in each subgrid */

	for (v = 0; v < DIM; v++) {
		for (ig = 0; ig < COLS; ig++) {
			for (jg = 0; jg < ROWS; jg++) {
				count = 0;
				for (i = ig*ROWS; i < (ig+1)*ROWS; i++) {
					for (j = jg*COLS; j < (jg+1)*COLS; j++) {
						ind[count] = i*DIM*DIM + j*DIM + v;
						val[count] = 1.0;
						count++;
					}
				}

				error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) goto QUIT;
			}
		}
	}



	/* Optimize model */

	error = GRBoptimize(model);
	if (error) goto QUIT;

	/* get the solution - the assignment to each variable */

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) goto QUIT;

	if (optimstatus == GRB_INF_OR_UNBD || optimstatus != GRB_OPTIMAL){
		status = -1;
		goto QUIT;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) goto QUIT;

	/* 3-- number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, DIM*DIM*DIM, sol);
	if (error) {
				goto QUIT;
	}

	QUIT:

	/* Error reporting */

	if (error) {
		printf("ERROR: %s\n", GRBgeterrormsg(env));
		free(board);
		if(model != NULL){
			free(model);
		}
		if(env != NULL){
			free(env);
		}
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(sol);

		return -2;
	}

	if(status == 0 && solution_board != NULL) {
		/* set solution board with given solution */
		for (i = 0; i < DIM; i++) {
			for (j = 0; j < DIM; j++) {
				for (v = 0; v < DIM; v++) {
					if(sol[i*DIM*DIM + j*DIM + v]) {
						solution_board[i*DIM + j] = v+1;
					}
				}
			}
		}
	}

	/* Free model */

	if(model != NULL){
		free(model);
	}
	if(env != NULL){
		free(env);
	}
	/* Free environment */
	free(ind);
	free(val);
	free(lb);
	free(vtype);
	free(sol);

	free(board);


	return status;
}

/*
 * calculates how many empty cells are in each row, in each column and in each block.
 * @param given_board: the current board
 * @param int* rows_empty : an array for all of the rows in the board
 * @param int* cols_empty : an array for all of the cols in the board
 * @param int* block_empty : an array for all of the blocks in the board
 * @param ROWS board dimensions
 * @param COLS board dimensions
 */

void count_empty_cells(int* given_board,int* rows_empty, int* cols_empty, int* block_empty, int ROWS, int COLS){
	int i,j;
	for(i=0; i< ROWS*COLS; i++){
		for(j=0;j<ROWS*COLS;j++){
			if(given_board[i*ROWS*COLS+j] != -1){
				rows_empty[i] --;
				cols_empty[j]--;
				block_empty[(i/ROWS)*ROWS +j/COLS]--;
			}
		}
	}
}
/*
 * @param given_board: the current board
 * @param solution_board : the solution board to return
 * @param ROWS: board dimensions
 * @param COLS board dimensions
 */
int LP(int* given_board, double* solution_board, int ROWS, int COLS)
{
	int DIM = ROWS*COLS;
	int status = 0;
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;

	int* 		board = (int*) malloc(DIM*DIM*sizeof(int));
	int*       ind = (int*) malloc(DIM*sizeof(int));
	double*     val = (double*) malloc(DIM*sizeof(double));

	double*     lb = (double*) malloc(DIM*DIM*DIM*sizeof(double));
	char*     vtype = (char*) malloc(DIM*DIM*DIM);
	double*     sol = (double*) malloc(DIM*DIM*DIM*sizeof(double));


	double* obj = (double*) malloc(DIM*DIM*DIM*sizeof(double));


	int       optimstatus;
	double    objval;
	int       i, j, v, ig, jg, count;
	int       error = 0;

	int* rows_empty = (int*) malloc(ROWS*COLS*sizeof(int));
	int* cols_empty = (int*) malloc(ROWS*COLS*sizeof(int));
	int* block_empty = (int*) malloc(ROWS*COLS*sizeof(int));

	double possibleCoeff;
	/* init degrees of freedom for all arrays */

	for(i=0; i<DIM; i++) {
		rows_empty[i] = ROWS*COLS+1 ;
		cols_empty[i] = ROWS*COLS+1 ;
		block_empty[i] = ROWS*COLS+1 ;
	}

	copyBoard(given_board, board, DIM*DIM);
	for (i=0; i<DIM; i++){
		for(j=0; j<DIM; j++){
			board[i*DIM + j] -= 1;
		}
	}


	count_empty_cells(board,rows_empty,cols_empty,block_empty,ROWS,COLS);

	/* Create environment */

	error = GRBloadenv(&env, "sudoku.log");
	if (error) goto QUIT;

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	/* Create new model */


	/* set lower bounds on cells already filled with a value + set all variable types to continous */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				if (board[i*DIM+ j] == v)
					lb[i*DIM*DIM+j*DIM+v] = 1;
				else
					lb[i*DIM*DIM+j*DIM+v] = 0;
				vtype[i*DIM*DIM+j*DIM+v] = GRB_CONTINUOUS;

			}
		}
	}
	/* create an empty model */
	error = GRBnewmodel(env, &model, "sudoku", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) goto QUIT;
	/* init the objective array with the calculation of coefficients as explained in header file */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				possibleCoeff = (float)1/(rows_empty[i]+1) + (float)1/(cols_empty[j]+1) +(float)1/(block_empty[(i/ROWS)*ROWS +j/COLS]+1);

				obj[i*DIM*DIM+j*DIM+v] = possibleCoeff + (float)0.5/(1+(rand()%(ROWS*COLS)));
			}
		}
	}


	free(rows_empty);
	free(cols_empty);
	free(block_empty);

	/* add variables to model, including obj: objective function coefficients, lb: lower bounds set to 1 for filled cells, vtype: continoues*/
	error = GRBaddvars(model, DIM*DIM*DIM, 0, NULL, NULL, NULL, obj, lb, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		error =2;
		goto QUIT;
	}

	/* Change objective sense to maximizations */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		error =2;
		goto QUIT;
	}

	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		error =2;
		goto QUIT;
	}


	/*CONSTRAINTS */
	/*1) Each cell gets a value */

	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				ind[v] = i*DIM*DIM + j*DIM + v;
				val[v] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("error in adding constraint of each cell getting a value \n");
				error = 2;

				goto QUIT;
			}
		}
	}
	/* range constraints */


	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				ind[0] = i*DIM*DIM + j*DIM + v;
				val[0] = 1.0;

				error = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
				if (error) {
					printf("error in adding constraint of range, smaller than 1 \n");
					error = 2;

					goto QUIT;
				}
				error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
				if (error) {
					printf("error in adding constraint of range, greater than 0 \n");
					error = 2;

					goto QUIT;
				}

			}

		}
	}
	/* Each value must appear once in each row */

	for (v = 0; v < DIM; v++) {
		for (j = 0; j < DIM; j++) {
			for (i = 0; i < DIM; i++) {
				ind[i] = i*DIM*DIM + j*DIM + v;
				val[i] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("error in adding constraint of each value must appear once in each row \n");
				error = 2;

				goto QUIT;
			}
		}
	}

	/* Each value must appear once in each column */

	for (v = 0; v < DIM; v++) {
		for (i = 0; i < DIM; i++) {
			for (j = 0; j < DIM; j++) {
				ind[j] = i*DIM*DIM + j*DIM + v;
				val[j] = 1.0;
			}

			error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("error in adding constraint of each value must appear once in each column \n");
				error = 2;

				goto QUIT;
			}
		}
	}

	/* Each value must appear once in each subgrid */

	for (v = 0; v < DIM; v++) {
		for (ig = 0; ig < COLS; ig++) {
			for (jg = 0; jg < ROWS; jg++) {
				count = 0;
				for (i = ig*ROWS; i < (ig+1)*ROWS; i++) {
					for (j = jg*COLS; j < (jg+1)*COLS; j++) {
						ind[count] = i*DIM*DIM + j*DIM + v;
						val[count] = 1.0;
						count++;
					}
				}

				error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) {
					printf("error in adding constraint of each value must appear once in block \n");
					error = 2;

					goto QUIT;
				}
			}
		}
	}


	/* Optimize model */

	error = GRBoptimize(model);
	if (error) {
		printf("error in optimization of gurobi \n");
		error = 2;

		goto QUIT;
	}

	/* Capture solution information */

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("error in obtaining optimum status of Gurobi \n");
		error = 2;
		goto QUIT;
	}

	if (optimstatus == GRB_INF_OR_UNBD || optimstatus != GRB_OPTIMAL){
		status = -1;
		goto QUIT;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		printf("error in obtaining value of objective function \n");
		error = 2;
		goto QUIT;
	}

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, DIM*DIM*DIM, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		error = 2;
	}



	QUIT:

	/* Error reporting */

	if (error == 2) {
		printf("ERROR: %s\n", GRBgeterrormsg(env));
		if(model != NULL){
			free(model);
		}
		if(env != NULL){
			free(env);
		}
		free(board);
		free(ind);
		free(val);
		free(lb);
		free(vtype);
		free(sol);
		free(obj);
		return -2;
	}


	copyFloatBoard(sol, solution_board, DIM*DIM*DIM);
	free(board);
	free(ind);
	free(val);
	free(lb);
	free(vtype);
	free(sol);
	free(obj);
	if(model != NULL){
		free(model);
	}
	if(env != NULL){
		free(env);
	}

	return status;
}
