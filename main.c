/* main summary:
 *
 * responsible for the main loop of all game instances
 *
 * according to return code of play_game:
 * 0 - valid exit
 * 1 - go to init game (restart)
 * -1 - unhandled error & exit
 */

#include <stdlib.h>
#include "Solver.h"
#include "Game.h"
#include "Parser.h"
#include <stdio.h>
#include <time.h>
#include "main_aux.h"
#include <time.h>

int main() {
	int result;
	srand(time(0));
	printf("Welcome to our SUDOKU GAME! \n");
	result = play_game();
	if(result==0){
	return 0;
	}
	else {
	return -1;
	}
}
