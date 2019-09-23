/*
 * Error Print module summary:
 * includes many functions to print different errors and messages to the user.
 */

#ifndef ERROR_PRINT_H_
#define ERROR_PRINT_H_

void printCommandUnavailable();
void printInvalidCommand(char* command);

void printFunctionError(char* function_name);

void printCellIs(int col ,int row, char* reason);

void printInvalidParameter(int command, char* given, char* wanted);
void printBoardIsTooFull(int wanted, int given);
void printInvalidParameterRangeFloat(int command, float given, char* wanted);
void printInvalidParameterFloat(int command, char* given, char* wanted);
void printInvalidParameterRangeInt(int command, int given, char* wanted);
void printWinMessage();
void printTooXArguments(int command, char* fewOrMany);

char* correctParamFormat(int command);
void printErrorSave(char* reason);

void printEmptyCommand();
void printBoardErroneous(int command);

void printCommandTooLong();

void printExit();
void printPrompt();

void printNullPath();


void printFileOpenError(char * path);

void printErrorFileFormat(char* reason);
void printErrorFileDimensions();


#endif /* ERROR_PRINT_H_ */
