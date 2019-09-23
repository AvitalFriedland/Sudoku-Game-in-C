/* Parser summary:
 * A module that is responsible for parsing the user commands.
 *
 * getCommand: a function that parses user input, including the command and the relevant parameters from the user. the function returns the command numerical representation (first char in results array, see numerical representation in Parser_aux).
 * the function changes the char* results, where the 1'st char represents the command, and the next chars represent the params. if the command has a string parameter (e.g. save or edit), the string is stored in results+1. if the command has int params (e.g. set), the number is stored in the next char objects in result (it is assumed that numbers are not higher than char limit). for float (guess command), it is stored in results+1, and parsed with a dedicated function from char* to float.
 * the function does not return if the command is not recognized, or if the params are not adequate, but rather prints error message, and awaits another command.
 *
 * readPath: a helper function that reads the path given as param for save/edit commands. path is stored in results+1, or results[0] is changed to -1 if there was an error (error is printed adjustantly with the state)
 *
 * readFloatParam: a helper function that reads the float value given as param for guess command. float is stored in results+1, or results[0] is changed to -1 if there was an error (error is printed adjustantly with the state)
 *
 * readParameters: a helper function that reads the numerical params given for commands. the values are stored in results+1 and on, or results[0] is changed to -1 if there was an error (e.g. too many/few params. error is printed adjustantly with the state)
 * */

#include <stdio.h>
#include "Parser.h"
#include <string.h>
#include <stdlib.h>
#include "Game.h"
#include "main_aux.h"
#include <ctype.h>
#include <math.h>
#include "Error_Print.h"
#include "Parser_aux.h"



/* state mapping:
 * 0 -> Init
 * 1 -> Edit
 * 2 -> Solve
 */

/*
 * getCommand: a function that parses user input, including the command and the relevant parameters from the user. the function returns the command numerical representation (first char in results array, see numerical representation in Parser_aux).
 * the function changes the char* results, where the 1'st char represents the command, and the next chars represent the params. if the command has a string parameter (e.g. save or edit), the string is stored in results+1. if the command has int params (e.g. set), the number is stored in the next char objects in result (it is assumed that numbers are not higher than char limit). for float (guess command), it is stored in results+1, and parsed with a dedicated function from char* to float.
 * the function does not return if the command is not recognized, or if the params are not adequate, but rather prints error message, and awaits another command.
 * @param int state: the state in which the game is currently at, which affects the relevancy of the commands parsed
 * @param char* results: a char array in which command and the parameters parsed are returned to the caller function
 * @return: void
 */
void getCommand(int state, char* results) {
	int lenInputRead;
	void* pointers[2];

	char* str = (char*) calloc(1024,1);
	char* command = (char*) malloc(20); /* 13 is enough */
	int pointersSize = 2;
	int i=0;

	pointers[0] = str;
	pointers[1] = command;

	/* handle allocation error */

	if (str == NULL || command==NULL ) {
		printFunctionError("calloc");
		results[0] = -2;
		return ;
	}


	while(1){
		/* get input stream into str */
		printPrompt();
		if(fgets(str, 1024, stdin) == NULL) {
			printFunctionError("fgets");
		}

		/* handle a command with too many chars */
		lenInputRead = strlen(str);
		if (lenInputRead>257){ /*last character is \n */
			printCommandTooLong();
			continue;
		}

		/* handle end of file, error reading and empty reading */
		if (feof(stdin)) {
			results[0] = 0;
			printExit();
			freeVariables(pointers, pointersSize);
			return ;
		}

		/* get command */
		command = strtok(str, " \t\r\n");


		/* handle empty command */
		if (command == NULL ){
			printEmptyCommand();
			continue;
		}

		results[0] = -1;
		for(i=0; i<17; i++) {
			if(strcmp(convertCommandToString(i), command) == 0) {
				results[0] = i;
				break;
			}
		}
		if(results[0] == -1) {
			printInvalidCommand(command);
			continue;
		}
		checkModeCompatibility(results,state);

		if(results[0]==-1) {
			printCommandUnavailable();
			continue;
		}


		/* read parameters with ints */

		switch(results[0]){
		case 5:
			readParameters(results, 3);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;

		case 8:
		case 12:
		case 13:
			readParameters(results, 2);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;
		case 3:
			readParameters(results, 1);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;

		case 7:
			readFloatParam(results);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;
		case 1:
		case 2:
		case 11:
			readPath(results);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;
		default:
			readParameters(results, 0);
			if(results[0]==-1){
				continue;
			}
			freeVariables(pointers,pointersSize);
			return ;
		}

	}
	freeVariables(pointers,pointersSize);
}

/*
 * readPath: a helper function that reads the path given as param for save/edit commands. path is stored in results+1, or results[0] is changed to -1 if there was an error (error is printed adjustantly with the state)
 * @param char* results: the results array in which path is stored if param is correct, or command that was stored in results[0] is changed to -1 to mark error
 * @return: void
 */
void readPath(char* results){
	char* tmpVal = strtok(NULL, " \t\r\n");
	int currentCommand = results[0];

	if(tmpVal==NULL){
		if(currentCommand==11 || currentCommand==1){ /*if command is solve or save we need a path*/
			printTooXArguments(currentCommand, "few");
			results[0]=-1;
			return;
		}
		else{ /*the command is 2 == edit so path is optional*/
			printf("herez \n");
			results[1] = '\0';
			return;
		}
	}
	strcpy((results+1), tmpVal);

	if(strtok(NULL, " \t\r\n")!=NULL){
		printTooXArguments(currentCommand, "many");
		results[0]=-1;
	}
}
/*
 * readFloatParam: a helper function that reads the float value given as param for guess command. float is stored in results+1, or results[0] is changed to -1 if there was an error (error is printed adjustantly with the state)
 * @param char* results: the results array in which the float is stored if param is correct, or command that was stored in results[0] is changed to -1 to mark error
 * @return: void
 */
void readFloatParam(char* results){
	int isIntegerOrFloat, i;
	float tmpFloat;
	char* tmpFloatInChars;
	char* tmpVal = strtok(NULL, " \t\r\n");
	if(tmpVal==NULL){
				printTooXArguments(7, "few");
				results[0]=-1;
				return;
	}
	isIntegerOrFloat = isValidInteger(tmpVal);
	if(isIntegerOrFloat == 0) {
		printInvalidParameterFloat(results[0], "string", "float");
		results[0] = -1;
		return;
	}

	tmpFloat = atof(tmpVal);
	tmpFloatInChars = ( char* ) &tmpFloat;
	for(i=0; i<(int)sizeof(float); i++){
		results[i+1] = tmpFloatInChars[i];
	}

	if(strtok(NULL, " \t\r\n") !=NULL){
		printTooXArguments(results[0], "many");
		results[0]=-1;
		return;
	}

	if(tmpFloat > 1 || tmpFloat < 0){
		printInvalidParameterRangeFloat(results[0], tmpFloat, correctParamFormat(results[0]));
		results[0]=-1;
	}
}
/*
 * readParameters: a helper function that reads the numerical params given for commands. the values are stored in results+1 and on, or results[0] is changed to -1 if there was an error (e.g. too many/few params. error is printed adjustantly with the state)
 * @param char* results: the results array in which values are stored if params are correct, or command that was stored in results[0] is changed to -1 to mark error
 * @param int numParams: the number of params that should be parsed for the command
 * @return: void
 */
void readParameters(char* results, int numParams){
	int i =0;
	char* tmpVal = strtok(NULL, " \t\r\n");
	int currentCommand = 0;
	int isIntegerOrFloat = 0;
	char* badInputVal =  (char*) malloc(256);
	int badInputIndex = -1;
	int floatInputIndex = -1;
	char* floatInput = NULL;




	currentCommand = results[0];
	while (tmpVal != NULL && i < numParams) {
		isIntegerOrFloat = isValidInteger(tmpVal);
		if(isIntegerOrFloat==0) { /*not either*/
			results[0] = -1;
			if(badInputIndex==-1)  {strcpy(badInputVal, tmpVal); badInputIndex = i;}
		}
		if(isIntegerOrFloat==2) { /*isFloat*/
			if(floatInputIndex==-1)  {
				floatInput = (char*) malloc(256);
				strcpy(floatInput, tmpVal); floatInputIndex = i;}
		}
		results[i+1] = atof(tmpVal);
		i++;
		tmpVal = strtok(NULL, " \t\r\n");
	}
	if (i < numParams) {
		printTooXArguments(currentCommand, "few");
		results[0]=-1;
		free(badInputVal);
		return;
	}
	if(tmpVal !=NULL){
		printTooXArguments(currentCommand, "many");
		results[0]=-1;
		free(badInputVal);
		return;
	}

	checkCorrectRange(badInputVal, floatInputIndex, floatInput, results, currentCommand);
	free(badInputVal);
}


