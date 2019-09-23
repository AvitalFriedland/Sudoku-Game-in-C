/*
 * Error Print module summary:
 * includes many functions to print different errors and messages to the user.
 */
#include <stdio.h>
#include "Parser.h"
#include "Parser_aux.h"

#include <string.h>
#include <stdlib.h>
#include "Game.h"
#include "Error_Print.h"
#include <ctype.h>
#include <math.h>


void printFunctionError(char* function_name){
	printf("the function %s has failed. \n", function_name);

}
void printFileOpenError(char * path){
	printf("error opening file in path %s\n", path);
}

void printErrorFileFormat(char* reason){
	printf("Error reading file, it is not in the correct format because %s", reason);
}
void printErrorFileDimensions(){
	printf("Error reading file, because the dimensions of the board are not in the correct range \n");
}

void printCellIs(int col ,int row, char* reason){
	printf("the cell at index <%d,%d> is %s \n",col,row, reason);
}


void printCommandUnavailable(){
	printf("this command is unavailable in the current mode\n");
}
void printInvalidCommand(char* command){
	printf("The given command: \"%s\" does not exist\n", command);
}

void printInvalidParameter(int command, char* given, char* wanted){
	printf("the given parameter: %s is invalid (because it isn't an int). The correct type of parameter for command %s should be: %s \n",given,convertCommandToString(command), wanted);
}

void printInvalidParameterRangeFloat(int command, float given, char* wanted){
	printf("the given parameter: %.2f is invalid (because it is not in the correct range). The correct type of parameter for command %s should be: %s \n",given,convertCommandToString(command), wanted);
}

void printInvalidParameterRangeInt(int command, int given, char* wanted){
	printf("the given parameter: %d is invalid (because it is not in the correct range). The correct type of parameter for command %s should be: %s \n",given,convertCommandToString(command), wanted);
}

void printInvalidParameterFloat(int command, char* given, char* wanted){
	printf("the given parameter: %s is invalid (because it is a float). The correct type of parameter for command %s should be: %s \n",given,convertCommandToString(command), wanted);
}

void printTooXArguments(int command, char* fewOrMany){
	printf("You have entered too %s arguments for the given command: \"%s\". ",fewOrMany, convertCommandToString(command));
	printf("The correct format is: \"%s %s\" \n", convertCommandToString(command), correctParamFormat(command));
}

char* correctParamFormat(int command){
	switch (command) {
	case 1: /*solve */
		return ("PATH");
		break;
	case 3: /*mark errors */
		return ("{0 or 1 (Integers)}");
		break;
	case 5:
		return ("X Y Z (Integers)");
		break;
	case 7:
		return ("THRESHOLD (0<=Float<=1)");
		break;
	case 8:
		return ("X Y (Integers)");
		break;
	case 11:
		return ("PATH");
		break;
	case 9:
		return ("PATH");
		break;
	case 12:
		return ("X Y (Integers)");
		break;
	case 13:
		return ("X Y (Integers)");
		break;
	}
	return ("ERROR");
}

void printWinMessage(){
	printf("CONGRATULATIONS! you have solved the board! switching to init mode. \n");

}
void printEmptyCommand(){
	printf("you have entered an empty command. try again! \n");
}
void printBoardIsTooFull(int wanted, int given){
	printf("Could not fill %d cells, because there are only %d empty cells in the board.\n", wanted, given);

}


void printCommandTooLong(){
	printf("you have entered too many characters for a single command. try again!\n");

}

void printBoardErroneous(int command){
	printf("The given command %s cannot be executed because the board contains errors.\n", convertCommandToString(command));

}

void printErrorSave(char* reason){
	printf("could not save the board to file because %s. \n", reason);
}
void printExit(){
	printf("Exiting..\n");
}

void printPrompt(){
	printf("Enter a command to play!\n");
}

void printNullPath(){
	printf("error reading path\n");
}
