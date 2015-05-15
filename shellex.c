/* Luis Enrique Ramirez, Geoffrey Tucker, Allen Thich, Miles Bonner  */
/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128
#define LESS_THAN "<"
#define GREATER_THAN ">"
#define EMPTY_STRING ""

/* function prototypes */
void eval(char *cmdline);
void parseIORedirection (char **argv, char **newArgv, char* inputFile, char* outputFile);
void redirectIO(char* inputFileName, char* outputFileName);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 


int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
		/* Read */
		printf("> ");                   
		fgets(cmdline, MAXLINE, stdin); 
		if (feof(stdin))
		    exit(0);

		/* Evaluate */
		eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char *filteredArgv[MAXARGS];
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
	char inputFileName[MAXLINE];
	char outputFileName[MAXLINE];

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    parseIORedirection(argv, filteredArgv, inputFileName, outputFileName);

    if (filteredArgv[0] == NULL)
		return;   /* Ignore empty lines */

    if (!builtin_command(filteredArgv)) { 
		if ((pid = fork()) == 0) {   /* Child runs user job */
    		redirectIO(inputFileName, outputFileName);
		    if (execve(filteredArgv[0], filteredArgv, environ) < 0) {
				printf("%s: Command not found.\n", filteredArgv[0]);
				exit(0);
		    }
		}

		/* Parent waits for foreground job to terminate */
		if (!bg) {
		    int status;
		    if (waitpid(pid, &status, 0) < 0)
				error("waitfg: waitpid error");
		}
		else
		    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit")) /* quit command */
		exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
		return 1;
    return 0;                     /* Not a builtin command */
}
/* $end eval */

void redirectIO(char* inputFileName, char* outputFileName)
{
	FILE* inputFile;
	FILE* outputFile;

	if (strcmp(inputFileName, EMPTY_STRING)) {
		inputFile = fopen(inputFileName, "r");

		if (inputFile != NULL){
			dup2(fileno(inputFile), STDIN_FILENO);
		}
	}

	if (strcmp(outputFileName, EMPTY_STRING)){
		outputFile = fopen(outputFileName, "w+");

		if (outputFile != NULL){
			dup2(fileno(outputFile), STDOUT_FILENO);
		}
	}

}

void parseIORedirection (char **argv, char **newArgv, char* inputFile, char* outputFile)
{
	int i = 0;
	int newArgvIndex = 0;
	int parsedInput = 0;
	int parsedOutput = 0;

	/* Clear file names */
	strcpy(inputFile, EMPTY_STRING);
	strcpy(outputFile, EMPTY_STRING);

    while (argv[i] != NULL) {
    	if (strcmp(argv[i], LESS_THAN) == 0) { /*Input redirection*/
     		if (argv[i+1] != NULL && !parsedInput){
				strcpy(inputFile, argv[i+1]);
				++parsedInput;
			}
    	} else if (strcmp(argv[i], GREATER_THAN) == 0) { /*Output redirection*/
    		if (argv[i+1] != NULL && !parsedOutput){
				strcpy(outputFile, argv[i+1]);
				++parsedOutput;	
    		}
    	}
    	++i;
    }
    i = 0;
    /* *newArgv = malloc(sizeof(char*) * (newArgvSize));*/
    while(argv[i] != NULL){
    	if (strcmp(argv[i], LESS_THAN) == 0 
    	||  strcmp(argv[i], GREATER_THAN) == 0) {
    		if (argv[i+1] != NULL){
    		 	++i;
    		}
    	} else {
    		newArgv[newArgvIndex] = argv[i];
    		++newArgvIndex;
    	}
    	++i;
	}
    newArgv[newArgvIndex] = NULL;
}

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
	       buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */


