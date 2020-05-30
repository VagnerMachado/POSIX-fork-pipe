/*

 Vagner Machado - QCID 23651127

 This program aims is to utilize the fork(), pipe(), execlp(), gettimeofday(), wait() system calls
 to build a program that accomplishes the following tasks:

 0 - Validate user input, print message if error found and exit.
 1 - Create a pipe
 2 - Parent forks a new process
 	 2.1 - Parent waits
 3 - Child process executes:
 	 3.1 - Record current time
 	 3.2 - Display current seconds and microseconds
 	 3.3 - Write seconds and microseconds into pipe
 	 3.4 - Execute function passed as parameter by the user.
 	 3.5 - Print information if execlp fails and exit the program
 4 - Parent checks exit status for the child, if error, exits the program.
 	 4.1 - Parent records the current time
 	 4.1 - Parent displays its end time for seconds and microseconds
 	 4.2 - Parent displays the start time seconds and microseconds received from child process
 	 4.3 - Time elapsed is calculated and displayed on the console
 5 - Successful run achieved;

 */

//  time.c
//  Project_1
//
//  Created by Vagner Machado on 2/15/20.
//  Copyright © 2020 Vagner Machado. All rights reserved. Nah, You can use it anywhere!


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#define BUFFER_LENGTH 50    //buffer length for information sent from child to parent
#define PIPE_READ_SIDE 0    //defining reading end of pipe
#define PIPE_WRITE_SIDE 1   //defining writing end of the pipe
#define CHOICE_LENGTH 5     //the count of parameters accepted for the program

//declaration of function that executes upon input error of --help
void printWarningMessage(const char *, const char **);

//error created a pipe then a fork, must be fork then pipe
int main(int argc, const char * argv[])
{
	//an array with valid input options
	const char * options [CHOICE_LENGTH] = {"ls","ps","whoami","hostname","date"};

	//error: missing input parameter
	if(argc == 1)
		printWarningMessage("BLANK", options);

	//user requires usage help message
	if(argc > 1 && strcmp(argv[1], "--help") == 0)
		printWarningMessage("--help", options);

	//print command passed as parameter
	printf("Name of command: %s\n", argv[1]);

	//declaring some variables to be used throughout the program
	struct timeval current;                     //struct to receive the start time
	struct timeval later;                       //struct to receive the end time
	char secondsSentFromChild[BUFFER_LENGTH];   //buffer for second read
	char microsecsSentFromChild[BUFFER_LENGTH]; //buffer for microsec read
	int fd[2];                  //the file descriptor to be used to create a pipe
	int pipeCreate = pipe(fd);  //create the pipe
	pid_t pid = fork();         //for a new process   NOTE: pipe then fork, not the opposite.

	// catch error in creation of fork and pipe instantiations
	if( pid < 0 || pipeCreate < 0)
	{
		//instantiate a couple of error messages
		char forkID [] = "ERROR: Could not fork() to create a process ID for child process\n";
		char procID [] = "Could not create a pipe() with the file descriptor";

		//checks if both fork and pipe failed
		if(pid < 0 && pipeCreate < 0)
			printf("%s%s", forkID, procID);
		//checks if only fork failed
		else if (pid < 0)
			printf("%s", procID);
		//else fork failed
		else
			printf("%s", forkID);
		//exit with error value
		exit(-1);
	}

	/*
     parent task closes its write end of pipe and then waits for the child to be done
     before it executes the tasks in the comments below
	 */
	else if (pid > 0)
	{
		//close the write end and wait for child
		close(fd[PIPE_WRITE_SIDE]);

		//wait for child to finish its task
		int status;
		wait(&status);

		//http://man7.org/linux/man-pages/man2/waitid.2.html
		//Tried to use WIFEXITED, WEXITSTATUS to catch bad exelp at child.
		//This works in my pc when executed using the terminal.

		//printf("\n**** wifexited     %d", WIFEXITED(status));   // 1 for child exited
		//printf("\n**** wexitstatus   %d", WEXITSTATUS(status)); // 0 for good execution of child, 255 for bad

		if(WIFEXITED(status) && WEXITSTATUS(status) == 0) //status is zero good good run
		{
			//register the finish time as soon as wait period is over.
			gettimeofday(&later, NULL);

			//read pipe to access the information written by child, close pipe read side
			read(fd[PIPE_READ_SIDE], secondsSentFromChild, BUFFER_LENGTH);
			read(fd[PIPE_READ_SIDE], microsecsSentFromChild, BUFFER_LENGTH);
			close(fd[PIPE_READ_SIDE]);

			//parse the information sent by the child process
			long secChild = atol(secondsSentFromChild);
			int microsecChild = atoi(microsecsSentFromChild);

			//display the end time set in parent process
			printf("\nEnd Time Seconds set in Parent:      %ld\n", later.tv_sec);
			printf("End Time microseconds set in Parent: %d\n\n", later.tv_usec);

			//display the start time set in child process
			printf("Start Time Seconds from Child in Parent:      %ld\n", secChild);
			printf("Start Time Microseconds from Child in Parent: %d\n\n", microsecChild);

			//perform necessary arithmetic and display the time elapsed
			long secDiff = later.tv_sec - secChild;
			long microsecDiff = later.tv_usec - microsecChild;
			if(secDiff > 0) //accounts for the seconds
				printf("Time Elapsed: %ld seconds and %ld microseconds\n", secDiff, (secDiff * 1000000) - microsecDiff);
			else
				printf("Time Elapsed: %ld microseconds\n\n", microsecDiff);

		}
		else
		{
			printf("\n\n *** Error in child process, parent process exited execution ***\n\n");
			return -1;
		}
	}
	/*
     The child process register the current time, sends seconds and microsecond into the
     pipe and then executes the parameter passed by the user.
	 */
	else
	{
		//register the current time and close the read side of pipe
		gettimeofday(&current, NULL);
		close(fd[PIPE_READ_SIDE]);

		//define buffers and write the seconds and microseconds into them
		char secBuffer [64];
		char microsecBuffer [64];
		sprintf(secBuffer, "%ld", current.tv_sec);
		sprintf(microsecBuffer, "%d", current.tv_usec);

		//display the buffer infomation
		printf("\nStart Time Seconds in Child:      %s\n", secBuffer);
		printf("Start Time Microseconds in Child: %s\n\n", microsecBuffer);

		//write buffer information into pipe and close the pipe write end
		write(fd[PIPE_WRITE_SIDE], secBuffer, BUFFER_LENGTH);
		write(fd[PIPE_WRITE_SIDE], microsecBuffer, BUFFER_LENGTH);
		close(fd[PIPE_WRITE_SIDE]);

		//executes and displays the output produces by the command
		printf("Command execution output:\n");
		char directory [100];

		/* In my computer, whoami does not print anything if it has /bin/ attached to first parameter,
		 * However if it does not have bin, it prints my name.
		 * I noticed that /bin/ keeps many other commands from running on MacOS such as uname, sw_vers ..
		 * Hence I decided to remove "/bin/" from first argument for execlp.
		 * If that does not help running in your machine, please uncomment if statement below an try again!
		 */
		//  if(strcmp(argv[1], "whoami") != 0) //original problem workaround
		//strcat(directory, "/bin/");
		strcat(directory,argv[1]);

		execlp(directory, argv[1], NULL); //this is last line of execution for child if execlp works

		//this is interesting: if the code does not return after the exelp call, an error occurred
		printf("\n** ERROR: Could not run \'execlp\' in child process with provided parameter <%s%s", argv[1], "> **");
		printWarningMessage(argv[1], options);
		//exit(0);
	}
	return 0;
}

/**
 printWarningMessage - a multipurpose function that prints warning messages onto the
 console case the user:
 1. does not pass a parameter
 2. passes an invalid paramenter
 3. passes --help as parameter argv[1]
 */
void printWarningMessage(const char * choice,const char ** validChoices)
{
	//checks if wrong parameter or no parameter was passed
	if(strcmp(choice, "--help") != 0)
		printf("\n*** ERROR: <%s> is not a valid parameter for this program ***\n", choice);

	//else the user requested help
	else
		printf("\n*** Here are some helpful tips ***\n");

	// The lines below print relevant information for the proper execution of program
	printf("\nThe program expects parameters such as the following, run as ./<appName> <parameter>\n");
	for(int i = 0; i < CHOICE_LENGTH; i++)
	{
		printf("\t%s\n", validChoices[i]);
	}
	printf("\nEXAMPLE:\n\tStep 1 - While in the directory with .c file, compile the program: gcc -c time.c <ENTER>\n");
	printf("\tStep 2 - Make executable with object file: gcc -o time.exe time.o <ENTER>\n");
	printf("\tStep 3 - Run the program: ./time.exe <parameter> <ENTER>\n\n");
	exit(-1);
}

