## POSIX-fork-pipe    

---   

A simple project that explores system calls, input validation and inter-process communication of a child and parent process through a POSIX pipe.   
Must be used in a device that supports POSIX system calls, i.e MacOS, Linux.    

---  
## Prompt
Write a C program called time.c that determines the amount of time necessary to run a command from the command line. This program will be run as “./time <command>” and will report the amount of elapsed time to run the specified command. This will involve using fork() and exec() functions, as well as the gettimeofday() function to determine the elapsed time. It will also require the use of two different IPC mechanisms.  

The general strategy is to fork a child process that will execute the specified command. However, before the child executes the command, it will record a timestamp of the current time (which we term “starting time”). The parent process will wait for the child process to terminate. Once the child terminates, the parent will record the current timestamp for the ending time. The difference between the starting and ending times represents the elapsed time to execute the command. As the parent and child are separate processes, they will need to arrange how the starting time will be shared between them.  

This program uses a pipe to share information between child and parent processes. The child will write the starting time to the pipe, and the parent will read from it following the termination of the child process.

gettimeofday() function is used to record the current timestamp. This function is passed a pointer to a struct timeval object, which contains two members: tv_sec and t_usec. These represent the number of elapsed seconds and microseconds since January 1, 1970. For IPC between the child and parent processes, the contents of the shared memory pointer can be assigned the struct timeval representing the starting time. When pipes are used, a pointer to a struct timeval can be written to and read from the pipe.    

---  

## Algorithm

This program aims is to utilize the fork(), pipe(), execlp(), gettimeofday(), wait() system calls 
to build a program that accomplishes the following tasks:
* Validate user input, print message if error found and exit.
	* Create a pipe
* Parent forks a new process
	* Parent waits
* Child process executes
	* Record current time
	* Display current seconds and microseconds
	* Write seconds and microseconds into pipe
	* Execute function passed as parameter by the user.
	* Print information if execlp fails and exit the program
* Parent checks exit status for the child, if error, exits the program.
	* Parent records the current time
	* Parent displays its end time for seconds and microseconds
	* Parent displays the start time seconds and microseconds received from child process
	* Time elapsed is calculated and displayed on the console
* Successful run achieved

**Vagner Machado**
