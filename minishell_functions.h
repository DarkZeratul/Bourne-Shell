/* 100277865 - dribeirobarbos00 - Douglas Ribeiro Barbosa
 * Dr. Ardeshir Bagheri
 * CPSC 2280 - 001
 * February 05, 2018
 */

#ifndef MINISHELL_FUNCTIONS_H_
#define MINISHELL_FUNCTIONS_H_

#define MAX_ARGS 		64
#define MAX_ARGS_LEN 	64
#define MAX_PATHS 		64
#define MAX_PATHS_LEN 	96
#define WHITESPACE 		" ,\t"

/** Prints the prompt string to stdout.
 */
extern void printPromt();

/** Checks whether the commands "exit" or "quit" have been issued.
 * 	If the the commands "exit" or "quit" have been issued returns 1.
 * 	Otherwise, returns 0.
 */
extern int wasTerminated (char* cmd);

/**
 * Returns ALL the commands issued by the user.
 */
extern char** get_cmds(char* cmd_line, int* number_of_cmds);

/**
 * Returns 0 if the cmd is supposed to run on the background. Returns -1 Otherwise.
 */
extern int isBackground(char* cmd_line);

/**
 * Returns the name of the redirected output file.
 */
extern char* get_redirectedOput(char* cmd_line);

/**
 * Returns the name of the redirected input file.
 */
extern char* get_redirectedIput(char* cmd_line);

/** Checks whether the calling process can access a file pathname in the environment list.
 * 	Returns the file pathname in the environment list of the calling process if the latter can access the former.
 * 	Otherwise, returns null pointer.
 */
extern char* get_pathName(char* paths, char* cmd);

/** Parse a string (mini-shell's argument) into a string array.
 */
extern char** parse_args (char* args);

/**
 * Set up the Redirect Input File Descriptors.
 * Returns the FID of stdin if Successful.
 * Otherwise, Returns -1 if Redirect Input is not required or -2 if there is an error throughout this function.
 */
int set_redirectedIput(char* iput);

/**
 * Set up the Redirect Output File Descriptors.
 * Returns the FID of stdout if Successful.
 * Otherwise, Returns -1 if Redirect Output is not required or -2 if there is an error throughout this function.
 */
int set_redirectedOput(char* oput);

#endif /* MINISHELL_FUNCTIONS_H_ */
