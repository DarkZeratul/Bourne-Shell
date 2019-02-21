/* 100277865 - dribeirobarbos00 - Douglas Ribeiro Barbosa
 * Dr. Ardeshir Bagheri
 * CPSC 2280 - 001
 * February 05, 2018
 */

#include <unistd.h>		// access, fork, execve.
#include <sys/wait.h>	// wait.
#include <sys/types.h>	// Datatype: pid_t.
#include <stdio.h> 		// fgets, printf, perror.
#include <stdlib.h>		// getenv, malloc, free.
#include <fcntl.h>
#include "minishell_functions.h"

// ### Constant Values.

#define LINE_LEN 		80
#define READ_END		0
#define WRITE_END		1

/** Basic Shell Program in C.
 *  The program behaves like a simplified version of the Bourne Shell.
 */
int main (int argc, char* argv[])
{
	//const int argsLength = (MAX_ARGS * MAX_ARGS_LEN) + 1; // +1: Terminating Character.
	char cmd_line[LINE_LEN];

	char* ptrPaths = getenv ("PATH"); // ! DO NOT change it directly.

	while (1)
	{
		printPromt();
		fgets(cmd_line, LINE_LEN, stdin); // Reads in the user's command from stdin and save it in 'args'.
		//printf("### MARCO 01\n");

		int number_of_cmds = 0;
		char** cmds = get_cmds(cmd_line, &number_of_cmds); // Split the commands.
		//printf("### MARCO 02\n");

		int fd[2];
		if (number_of_cmds > 1) pipe(fd);

		int counter = 0;
		while (counter < number_of_cmds)
		{
			int runBackground= isBackground(cmds[counter]);
			char* oput = get_redirectedOput(cmds[counter]);
			char* iput = get_redirectedIput(cmds[counter]);
			argv = parse_args(cmds[counter]); // Parse 'args' (mini-shell's argument) into a 'argv'.

			if (wasTerminated(argv[0])) { exit(0); }

			char* pathName = get_pathName(ptrPaths, argv[0]);

			if (pathName != NULL)
			{
				// ### The calling process can access a file pathname in the environment list.

				int saved_stdi = set_redirectedIput(iput);
				int saved_stdo = set_redirectedOput(oput);
				if (saved_stdi == -2 || saved_stdo == -2) break;

				pid_t status;
				pid_t cpid = fork();
				if (cpid == -1)
				{
					perror(0); // Cannot Fork.
				}
				else if (cpid == 0)
				{
					if (counter == 0) // 1st cmd of the cmd_line being executed.
					{
						if (number_of_cmds > 1) // Set up File Descriptors if piped.
						{
							close(1);
							dup(fd[1]);
							close(fd[0]);
							close(fd[1]);
						}
						if (execve(pathName, argv, NULL) == -1)
						{
							perror(0); // Could not execve.
						}
						exit(42);
					}
					else // 2nd cmd of the cmd_line being executed.
					{
						if (number_of_cmds > 1) // Set up File Descriptors if piped.
						{
							close (0);
							dup(fd[0]);
							close(fd[0]);
							close(fd[1]);
						}
						if (execve(pathName, argv, NULL) == -1)
						{
							perror(0); // Could not execve.
						}
						exit(42);
					}
				}
				else
				{
					if (counter == 1)
					{
						close(fd[0]);
						close(fd[1]);
					}
					if (runBackground == -1) wait(&status); // Wait for Children if not running on Background.
				}

				// Reestablish stdin and stdout if changed for file redirtection.

				if (saved_stdo != -1)
				{
					close(1);
					dup(saved_stdo);
					close(saved_stdo);
				}

				if (saved_stdi != -1)
				{
					close(0);
					dup(saved_stdi);
					close(saved_stdi);
				}

			// ### Deallocate Memory.

				if (pathName != NULL) { free(pathName); }

				int idx = 0;
				while(argv[idx] != NULL)
				{
					free(argv[idx]);
					idx++;
				}
				free(argv);
			}
			else
			{
				// ### The calling process cannot access a file pathname in the environment list.
				perror(0);
			}

			if (iput != NULL) free(iput);
			if (oput != NULL) free(oput);


			if (cmds[counter] != NULL) free(cmds[counter]);
			counter++;
		}
		free(cmds);
	}
	return 0;
}
