#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

extern char **getline();


/*
 _______________________________________________________________________
| The function inputRedirect takes three arguments char **args, index   |
| of the file that we want to open, and an Int value for a flag to see  |
| when we need to to input redirection. This funtion does not return    |
| any value. The purpose of this functon is to do input redirection when|
| there is character "<" entered by user in the command line. Open file |
| and check for successful open file. Reset the flag for input          |
| redirection. Store stdin to fileIn and check to see if it was done    |
| successfully.                                                         |
|_______________________________________________________________________|

*/ 

void inputRedirect(char **args, int fileIndex, int flagRedirect){

    int fileIn;

    if(flagRedirect == 1){ /* check to see if we need to redirect input from a file */
                
        fileIn = open(args[fileIndex], O_RDONLY);
        if(fileIn < 0){
            fprintf(stderr, "Error: open file faild.\n");
            exit(1);
        }
        flagRedirect = 0;         /* reseting the flag */
        if(dup2(fileIn, 0) < 0)  /* store stdin in to fileIn */
            printf("Input redirection has failed.\n");
    }

    return;
}

/*
 ________________________________________________________________________
| The function outputRedirect takes three arguments char **args, index   |
| of the file that we want to open, and an Int value for a flag to see   |
| when we need to to output redirection. This funtion does not return    |
| any value. The purpose of this functon is to do output redirection when|
| there is character ">" entered by user in the command line. Open file  |
| and check for successful open file. Reset the flag for output          |
| redirection. Store stdout to fileOut and check to see if it was done   |
| successfully.                                                          |
|________________________________________________________________________|

*/ 

void outputRedirect(char **args, int fileIndex, int flagRedirect){
    
    int fileOut;

    if(flagRedirect == 1){
                
        fileOut = open(args[fileIndex], O_RDWR | O_CREAT | O_TRUNC, 0666);
        if(fileOut < 0){
            fprintf(stderr, "open file faild.\n");
            exit(1);
        }
        flagRedirect = 0;
        if(dup2(fileOut,1) < 0)
            printf("output redirection has failed.\n");  
    }

    return;
}

/*
 ___________________________________________________________
| The function pipeCommand takes in two arguments           |
| char **args and char **rightArgs. The purpose if this     |
| is to do pipe in the case that user enteres special       |
| character "|". The fuctionality of this function is       |
| similar to pipeCommandError(). The ucntion does not return|
| any value. Argument args is the entire agruments that     |
| is entered by user, and rightArgs are the arguments that  |
| are after "|". We fork a child process in this function   |
| in order to create a pipe between a child and its parent. |
|___________________________________________________________|

*/
void pipeCommand(char **args, char **rightArgs){
    int fd[2];
    int childpid;
    int childpid2;

    pipe(fd);

    if((childpid = fork()) < 0){
        perror("Error: on childpid\n");
    }

    else if(childpid == 0){ /* child */

        close(fd[0]);
        dup2(fd[1], 1);
        execvp(args[0], args);
        exit(1);
    }

    else{ /* parent */

        close(fd[1]);
        wait();
    }

    if((childpid2 = fork()) < 0){            /* create a child process if its return value is less than 0, there is an error */
        perror("Error: on childpid2\n");    /* printing out the error */
    }

    else if(childpid2 == 0){ /* child */

        close(fd[1]);                       /* closing stdout */
        dup2(fd[0], 0);
        execvp(rightArgs[0], rightArgs);    /* executing the first argument */
        exit(1);
    }
    else{ /* parent */

        close(fd[0]);                       /* closing stdin */
        wait();
    }

    return;
}

int main() {
    int status;    
    int i, pipeIt;
    int pid;
    int file_input_redirection = 0;  /* This is to store the file's index */
    int file_output_redirection = 0; /* This is to store the file's index */
    int indexAfterPipe = 0;
    int output_redir;                /* This is a flag for output redirection */
    int input_redir;                 /* This is a flag for input redirection */
    char **args;
    char **rightArgs;                /* This is to store arguments after pip "|" */
    

    while(1) {

        /* Initializing flags for output/input redirection and pip */
        pipeIt = 0;
        output_redir = 0;
        input_redir = 0;

        printf("[Shell]: ");

        args = getline();

        if(args[0] == NULL){ /* If nothing has entered by user. */
            printf("");
            continue;
        }

        if(!strcmp((args[0]), "exit")){ /* exit out of shell on command exit. */
            printf("Exiting...\n");
            exit(0);

        }

        for(i = 0; args[i] != NULL; i++) { /* Loop through all the arguments entered by user. */

        /* ======================================= checking for < > | commands ====================================== */

            if((strcmp(args[i], ">")) == 0){/* if there is special character ">" entered by the user */

                if(args[i+1] == NULL){            /* print an error message if the there no argument given after ">" */
                    fprintf(stderr, "Error: no file is given after the > symbol.\n");
                }
                file_output_redirection = i+1;   /* getting the index of the file */
                output_redir = 1;                /* setting the flag for output redirection */
                args[i] = NULL;                  /* setting ">" to NULL */
            }
           
            else if((strcmp(args[i], "<")) == 0){ /* if there is special character ">" entered by the user */

               if(args[i+1] == NULL){            /* print an error message if the there no argument given after ">" */
                    fprintf(stderr, "Error: no file is given after the > symbol.\n");
                }
                file_input_redirection = i+1;   /* getting the index of the file */
                input_redir = 1;                /* setting the flag for input redirection */
                args[i] = NULL;                 /* setting ">" to NULL */
            }

            else if((strcmp(args[i], "|")) == 0){   /* if there is special character "|" entered by the user */
                if(args[i+1] == NULL){              /* print an error message if the there no argument given after "|" */
                    fprintf(stderr, "Error: nothing after |\n");
                }
                indexAfterPipe = i+1;       /* getting the index of after "|" */
                rightArgs = &args[indexAfterPipe]; /* storing arguments after | to rightArgs */
                args[i] = NULL;             /* setting "|" to NULL */
                pipeIt = 1;                 /* setting a flag */
            }
        }


        pid = fork();      /* Create a child process */

        if(pid < 0){
            perror("Error: no child process has been created.\n");
        }

        else if(pid == 0){  /* Child process */


            inputRedirect(args, file_input_redirection, input_redir);   /* Input redirection */
            outputRedirect(args, file_output_redirection, output_redir);/* Output redirection*/

            if(pipeIt == 1){ /* if the flag for pipe is set, do pipe */
                pipeCommand(args, rightArgs);
                pipeIt = 0; /* set the flag back to zero */
                exit(1);
            }
            
            if(execvp(args[0], args) < 0){   /* here is where args get executed - if return value is -1 print error*/       
                printf("Error on execvp\n"); /* if execvp is < 0 the print an error message */
                exit(1);
                }
            
        }
        else{        /* pid > 0 */

            waitpid(-1, &status,0);
        }
    }
}   
