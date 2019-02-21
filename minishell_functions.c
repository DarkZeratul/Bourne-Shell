/* 100277865 - dribeirobarbos00 - Douglas Ribeiro Barbosa
 * Dr. Ardeshir Bagheri
 * CPSC 2280 - 001
 * February 05, 2018
 */

#include <unistd.h>		// access, fork, execve.
#include <stdio.h> 		// fgets, printf, perror.
#include <stdlib.h>		// getenv, malloc, free.
#include <string.h> 	// strncmp, strtok, strpbrk, strchr, strcat.
#include <fcntl.h>
#include "minishell_functions.h"

/**
 * Returns 0 if the cmd is supposed to run on the background. Returns -1 Otherwise.
 */
int isBackground(char* cmd_line)
{
	char* ptrLstChar = strchr(cmd_line, '\0');
	if (*(ptrLstChar - 1) == '&')
	{
		*(ptrLstChar - 1) = '\0';
		return 0;
	}
	return -1;
}

/**
 * Set up the Redirect Input File Descriptors.
 * Returns the FID of stdin if Successful.
 * Otherwise, Returns -1 if Redirect Input is not required or -2 if there is an error throughout this function.
 */
int set_redirectedIput(char* iput)
{
	int saved_stdin, iFID;

	if (iput != NULL)
	{
		saved_stdin = dup(0);

		iFID = open(iput, O_RDONLY);
		if (iFID == -1)
		{
			perror(""); // Could not open the input file.
			return -2;
		}

		close(0);
		dup(iFID);
		close(iFID);

		return saved_stdin;
	}
	return -1;
}

/**
 * Set up the Redirect Output File Descriptors.
 * Returns the FID of stdout if Successful.
 * Otherwise, Returns -1 if Redirect Output is not required or -2 if there is an error throughout this function.
 */
int set_redirectedOput(char* oput)
{
	int saved_stdout, oFID;

	if (oput != NULL)
	{
		saved_stdout = dup(1);

		remove(oput);
		oFID = open(oput, O_WRONLY | O_CREAT);
		if (oFID == -1)
		{
			perror(""); // Could not create the input file.
			return -2;
		}

		close(1);
		dup(oFID);
		close(oFID);

		return saved_stdout;
	}
	return -1;
}

/**
 * Returns the name of the redirected input file.
 */
char* get_redirectedIput(char* cmd_line)
{
	char* ptrFstChar;
	char* ptrLstChar = strchr(cmd_line, '<');

	if (ptrLstChar != NULL)
	{
		*ptrLstChar = '\0';

		ptrFstChar = ptrLstChar + 1;
		ptrLstChar = strchr(ptrFstChar, '\0');

		while (*ptrFstChar == ' ') ptrFstChar += 1; // Removes Leading Whitespace.
		while (*(ptrLstChar - 1) == ' ') ptrLstChar -= 1; // Removes Trailing Whitespace.

		char* input = (char*)malloc((ptrLstChar - ptrFstChar) + 1);
		strncpy(input, ptrFstChar, ptrLstChar - ptrFstChar);
		input[ptrLstChar - ptrFstChar] = '\0'; // Inserts '\0' Terminator.

		return input;
	}
	return NULL;
}

/**
 * Returns the name of the redirected output file.
 */
char* get_redirectedOput(char* cmd_line)
{
	char* ptrFstChar;
	char* ptrLstChar = strchr(cmd_line, '>');

	if (ptrLstChar != NULL)
	{
		*ptrLstChar = '\0';

		ptrFstChar = ptrLstChar + 1;
		ptrLstChar = strchr(ptrFstChar, '\0');

		while (*ptrFstChar == ' ') ptrFstChar += 1; // Removes Leading Whitespace.
		while (*(ptrLstChar - 1) == ' ') ptrLstChar -= 1; // Removes Trailing Whitespace.

		char* output = (char*)malloc((ptrLstChar - ptrFstChar) + 1);
		strncpy(output, ptrFstChar, ptrLstChar - ptrFstChar);
		output[ptrLstChar - ptrFstChar] = '\0'; // Inserts '\0' Terminator.

		return output;
	}
	return NULL;
}

/** Prints the prompt string to stdout.
 */
void printPromt()
{
	const char* promptString = promptString = "mini-shell>";
	printf("%s", promptString);
}

/**
 * Returns ALL the commands issued by the user.
 */
char** get_cmds(char* cmd_line, int* number_of_cmds)
{
	char** cmds = (char**)malloc(strlen(cmd_line));

	char* ptrFstChar = cmd_line;
	char* ptrLstChar = strchr(cmd_line, '|');

	while (ptrLstChar != NULL)
	{
		cmds[*number_of_cmds] = (char*)malloc((ptrLstChar - ptrFstChar) + 1);
		strncpy(cmds[*number_of_cmds], ptrFstChar, ptrLstChar - ptrFstChar);
		cmds[*number_of_cmds][ptrLstChar - ptrFstChar] = '\0'; // Inserts '\0' Terminator.

		*(number_of_cmds) += 1;

		ptrFstChar = ptrLstChar + 1;
		ptrLstChar = strchr(ptrFstChar, '|');
	}

	ptrLstChar = strchr(cmd_line, '\n');

	cmds[*number_of_cmds] = (char*)malloc((ptrLstChar - ptrFstChar) + 1);
	strncpy(cmds[*number_of_cmds], ptrFstChar, ptrLstChar - ptrFstChar);
	cmds[*number_of_cmds][ptrLstChar - ptrFstChar] = '\0'; // Inserts '\0' Terminator.

	*(number_of_cmds) += 1;

	return cmds;
}

/** Checks whether the commands "exit" or "quit" have been issued.
 * 	If the the commands "exit" or "quit" have been issued returns 1.
 * 	Otherwise, returns 0.
 */
int wasTerminated (char* cmd)
{
	if (strncmp(cmd, "exit", 4) == 0 ||
		strncmp(cmd, "quit", 4) == 0)
	{
		return 1;
	}
	return 0;
}

/** Parse a string (mini-shell's argument) into a string array.
 */
char** parse_args(char* args)
{
	char** argv = (char**)malloc(MAX_ARGS);

	int numberOfArgs = 0;
	char* ptrBeginCh = args;
	char* ptrEndCh = strpbrk(args, WHITESPACE);

	while (ptrEndCh != NULL && numberOfArgs < MAX_ARGS)
	{
		if (ptrBeginCh != ptrEndCh)
		{
			argv[numberOfArgs] = (char*)malloc(MAX_ARGS_LEN + 1); // +1: Terminating Character.
			strncpy(argv[numberOfArgs], ptrBeginCh, ptrEndCh - ptrBeginCh);
			argv[numberOfArgs][ptrEndCh - ptrBeginCh] = '\0'; // Inserts '\0' Terminator.

			numberOfArgs++;
		}

		ptrBeginCh = ptrEndCh + 1;
		ptrEndCh = strpbrk(ptrBeginCh, WHITESPACE);
	}

	if (numberOfArgs < MAX_ARGS)
	{
		ptrEndCh = strchr(ptrBeginCh,'\0');

		if (ptrBeginCh != ptrEndCh)
		{
			argv[numberOfArgs] = (char*)malloc(MAX_ARGS_LEN + 1); // +1: Terminating Character.
			strncpy(argv[numberOfArgs], ptrBeginCh, ptrEndCh - ptrBeginCh);
			argv[numberOfArgs][ptrEndCh - ptrBeginCh] = '\0';
		}
	}
	return argv;
}

/** Checks whether the calling process can access a file pathname in the environment list.
 * 	Returns the file pathname in the environment list of the calling process if the latter can access the former.
 * 	Otherwise, returns null pointer.
 */
char* get_pathName(char* paths, char* cmd)
{
	char* pathName;
	pathName = (char*)malloc(MAX_PATHS_LEN + MAX_ARGS_LEN + 1); // +1: Terminating Character.

	int numberOfPaths = 0;
	char* ptrBeginCh = paths;
	char* ptrEndCh = strchr(paths,':');

	while (ptrEndCh != NULL && numberOfPaths < MAX_PATHS)
	{
		strncpy(pathName, ptrBeginCh, ptrEndCh - ptrBeginCh);
		pathName[ptrEndCh - ptrBeginCh] = '/';
		pathName[ptrEndCh - ptrBeginCh + 1] = '\0';
		strcat(pathName, cmd);

		if (access(pathName, X_OK) == 0) { return pathName; }

		numberOfPaths++;

		ptrBeginCh = ptrEndCh + 1;
		ptrEndCh = strchr(ptrBeginCh,':');
	}

	if (numberOfPaths < MAX_PATHS)
	{
		ptrEndCh = strchr(ptrBeginCh,'\0');

		strncpy(pathName, ptrBeginCh, ptrEndCh - ptrBeginCh);
		pathName[ptrEndCh - ptrBeginCh] = '/';
		pathName[ptrEndCh - ptrBeginCh + 1] = '\0';
		strcat(pathName, cmd);

		if (access(pathName, X_OK) == 0) { return pathName; }
	}
	return NULL;
}
