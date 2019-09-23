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
#ifndef PARSER_AUX_H_
#define PARSER_AUX_H_

char* convertCommandToString(int command);

void freeVariables(void** pointers, int size) ;

double parseFloatFromCharArray(char* address);


int isValidInteger(char* str);


void checkModeCompatibility(char* results, int state);


void checkCorrectRange(char* badInputVal, int floatInputIndex, char* floatInput, char* results, int currentCommand);




#endif /* PARSER_AUX_H_ */
