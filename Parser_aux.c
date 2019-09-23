/*
 * Parser_aux.c
 * contains helper functions to the Parser module. All regarding input from the user.
 * convetCommandToString: function converts the number representation of a command to it's adequate string
 * freeVariables: a function that frees all allocated memory objects stored in the pointers array
 * isValidInteger: a function that asserts the correctness of the string representation of a number: either invalid - 0, valid(int) - 1, float - 2
 * parseFloatFromCharArray: a helper function that parses a double from a float saved in a char array
 * checkModeCompatibility: a function that asserts the compatibility of the command to the state. if the command is not permitted in the state, results[0] is changed to -1
 * checkCorrectRange: a helper function that asserts the params are in the correct allowed range, and else prints error and changes results[0] to -1
 */
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


char* commandToString[] = {
		"exit", /*0 */
		"solve",  /*1 */
		"edit",  /*2 */
		"mark_errors",  /*3 */
		"print_board", /*4 */
		"set", /*5 */
		"validate", /*6*/
		"guess", /*7*/
		"generate", /*8*/
		"undo", /*9*/
		"redo", /*10*/
		"save", /*11*/
		"hint", /*12*/
		"guess_hint", /*13*/
		"num_solutions", /*14*/
		"autofill", /*15*/
		"reset" /*16*/
};

/*
 * convetCommandToString: function converts the number representation of a command to it's adequate string
 * @param int command: the number representing the command
 * @return: char*
 */
char* convertCommandToString(int command){
	return commandToString[command];
}

/*
 * freeVariables: a function that frees all allocated memory objects stored in the pointers array
 * @param void** pointers: an array of pointers to free
 * @param int size: the number of pointers to free
 * @return: void
 */
void freeVariables(void** pointers, int size) {
	while(size  > 0) {
		free(*pointers);
		pointers++;
		size--;
	}
}


/*
 * isValidInteger: a function that asserts the correctness of the string representation of a number: either invalid - 0, valid(int) - 1, float - 2
 * @param char* str: the number string to be checked
 * @return: int
 */
int isValidInteger(char* str){
	int valid = 0;
	int flagOneDot = 0;
	int i;
	int len = strlen(str);
	while (len > 0 && isspace(str[len - 1]))
		len--;     /* strip trailing newline or other white space */
	if (len > 0)
	{
		valid = 1;
		for (i = 0; i < len; ++i)
		{
			if (!isdigit(str[i] ))
			{
				valid = 0;
				if(str[i]=='.'&& flagOneDot ==0){
					valid = 2;
					flagOneDot=1;
				}
			}
		}
	}
	return valid;
}

/*
 * parseFloatFromCharArray: a helper function that parses a double from a float saved in a char array
 * @param char* address: the char array where the float is stored
 * @return: double
 */
double parseFloatFromCharArray(char* address){
	double res;
	float tmp;
	float* tmp_ptr;
	tmp_ptr = (float*)(address);
	tmp = tmp_ptr[0];
	res = (double) tmp;
	return res;
}


/*
 * checkModeCompatibility: a function that asserts the compatibility of the command to the state. if the command is not permitted in the state, results[0] is changed to -1
 * @param char* results: the char array where command and params are stored
 * @param int state: the state in which the game is in
 * @return: void
 */
void checkModeCompatibility(char* results, int state){
	int command = results[0];
	switch (command) {
	case 3:
	case 7:
	case 12:
	case 13:
	case 15:
		if(state !=2) {
			results[0]=-1;
			break;
		}
		break;

	case 4:
	case 5:
	case 6:
	case 10:
	case 11:
	case 9:
	case 14:
	case 16:
		if(state ==0) {
			results[0]=-1;
			break;
		}
		break;
	case 8:
		if(state !=1) {
			results[0]=-1;
			break;
		}
		break;
	}
}
/*
 * checkCorrectRange: a helper function that asserts the params are in the correct allowed range, and else prints error and changes results[0] to -1
 * @param char* badInputVal: the bad input string that needs to be fixed
 * @param int floatInputIndex: an index of the float in the string
 * @param char* floatInput: the float string to be parsed
 * @param char* results: the command and params array
 * @param int currentCommand: an indicator of the current command checked (in case results[0] changes to -1 and printing occures)
 * @return: void
 */
void checkCorrectRange(char* badInputVal, int floatInputIndex, char* floatInput, char* results, int currentCommand){
	double f;
	char* str_part;

	if(results[0]==-1){
		if(currentCommand==7){/* the command is guess so we expect a float */
			printInvalidParameter(currentCommand, badInputVal, "Float");
			return;
		}
		/* the command is expecting an integer */
		printInvalidParameter(currentCommand, badInputVal, "Integer");
		return;
	}
	switch(currentCommand){
	case 3: /* command is mark_errors */
		if(results[1]!=0 && results[1] !=1) {
			printInvalidParameterRangeInt(currentCommand, results[1], correctParamFormat(currentCommand));
			results[0]=-1;
		}
		break;
	case 7: /* command is guess */

		if(floatInput!=NULL){
			f = strtod(floatInput, &str_part);}
		else{
			f = results[1];
		}
		if(f>1.0 || f <0.0) {
			printInvalidParameterRangeFloat(currentCommand, f, correctParamFormat(currentCommand));
			results[0]=-1;
		}
		break;
	case 5:/* all of these commands expect an int */
	case 8:
	case 12:
	case 13:
		if(floatInputIndex!=-1){
			printInvalidParameterFloat(currentCommand, floatInput, correctParamFormat(currentCommand));
			results[0]=-1;
		}
		break;
	}
}


