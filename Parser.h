/* Parser summary:
 * A module that is responsible for parsing the user commands and call Game.c accordingly
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

#ifndef PARSER_H_
#define PARSER_H_

int parseUserCommands(int board[], int fixedBoard[], int solution[], int ROWS,
		int COLS, int numCellsFilled);
void getCommand(int state, char* results);
void readParameters(char* results, int numParams);
void readFloatParam(char* results);
double parseFloatFromCharArray(char* address);
void readPath(char* results);
char* correctParamFormat(int command);


#endif /* PARSER_H_ */
