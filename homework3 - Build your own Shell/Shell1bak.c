/* MCM Demo program for 615 lab #2
**	See the CSC 615 lecture notes slide #45 for details
**		Compile/execute under Linux using gcc
**
** 	Usage (you can substitute any command for ls):
**	> ./lab2demo ls
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
//***needed for exit() ***///
#include <stdlib.h>
//***to use boolean types***//
#include <stdbool.h>
//***for memset()***//
#include <string.h>
//***for uniform buffer size***//
#define BUFFER 256
#define SHELLOUT "Shell0 > "



main(int   argc, char *argv[]) {
	int	pid1, pid2, scantot;
	char end;
	int fd1;
	char myBuff[BUFFER];
	//char echo[BUFFER];
	int echoSize;
	int argsize;
	char * tokPtr;
	int	pipeID[2];
	int	status;
	//***for writing 
	char writeBuffer[BUFFER];
	int openFileDescriptor;
	int writeFileDescriptor;

	//argsize = argc;
	memset(&myBuff[0],'\0', BUFFER);	//clear myBuff
	
	///***begin loop***//
	do
	{
		//***read and tokenize input commands and place into argv***//
		write(1, SHELLOUT, sizeof(SHELLOUT));
		scantot = read(0, &myBuff[0], BUFFER);
		if(scantot<=0)
			break;
		else{
			//echoSize = sprintf(&echo[0], "You have entered %s", &myBuff[0]);
			//write(1, &echo, echoSize);
			argsize = 0;
			tokPtr = strtok(myBuff, " \n\0");	// make sure to do this outside of loop
			while(tokPtr != NULL){
				//***tokenize myBuff***//

				argv[argsize]=tokPtr;
				//http://www.cplusplus.com/reference/cstring/strtok/
				tokPtr = strtok(NULL, " \n\0");
				printf("%s\n",argv[argsize]);
				argsize++;
			}

			argv[argsize] = NULL;	//make sure final argv is null
		}
		//***if '&' found***//
		//argsize -1 or else segmentation fault
		if((strcmp(argv[argsize-1], "&")==0) && (argsize > 0)){
			printf("& found. running process in background \n");
			argv[argsize-1] = NULL;	//remove & from argv
			argsize--;
			//***program itself is parent***//
			//***clone process except for pid***//
			if ((pid1 = fork()) == -1 ) { /* error exit - fork failed */
				perror("Fork failed");
				exit(-1);
			}
			if (pid1 == 0) { /* this is the child */
				printf("This is the child ready to execute: %s\n",argv[0]);
				//execvp(argv[0],&argv[0]);
				execl(argv[0],argv[0],NULL);
				/* error exit - exec returned */
				perror("Exec returned");
				exit(-1);
			}
			if(pid1==0){}

			memset(&myBuff[0],'\0',BUFFER);	//clear myBuff
			//***clear argv***//
			//continue;

			}
		//***if > ***//
		else if((strcmp(argv[argsize-2], ">")==0)){
			if ((status = pipe(pipeID)) == -1) { /* error exit - bad pipe */
				perror("Bad pipe");
				exit(-1);
			}
			if ((pid1 = fork()) == -1) {  /* error exit - bad fork */
				perror("Bad fork");
				exit(-1);
			}
			if (pid1 == 0) { /* The first child process */
				/* make sure you start the last process in the pipeline first! */
				printf("The first child is starting: %s %s\n",argv[3],argv[4]);
				close (0);
				dup (pipeID[0]);
				close (pipeID[0]);
				close(pipeID[1]);
				execlp(argv[3],argv[3],argv[4],0); 
				/* error exit - exec returned */
				perror("Execl returned");
				exit(-1);
			}
			/* this is the parent again */
			if ((pid2 = fork()) == -1) { /* error exit - bad fork */
				perror("Bad fork");
				exit(-1);
			}
			if (pid2 == 0) { /* the second child process */
				printf("The second child is starting: %s %s\n",argv[1],argv[2]);
				close (1);
				dup(pipeID[1]);
				close(pipeID[0]);close(pipeID[1]);
				execlp(argv[1],argv[1],argv[2],0); 
				/* error exit - exec returned */
				perror("Execl returned");
				exit(-1);
			}
			/* back to the parent again */
			close(pipeID[0]);
			close(pipeID[1]);
			wait(pid1,0,0);
			wait(pid2,0,0);
			printf("The parent is exiting\n");
			
			
		}
		//------------------------------------------------------------------------------------------------		
		else{

			//***Dr Murphy's code***//
			//***program itself is parent***//
			if ((pid1 = fork()) == -1 ) { /* error exit - fork failed */
				perror("Fork failed");
				exit(-1);
			}
			//***Dr Murphy's code***//
			if (pid1 == 0) { /* this is the child */
				printf("This is the child ready to execute: %s\n",argv[0]);
				execvp(argv[0],&argv[0]);
				/* error exit - exec returned */
				perror("Exec returned");
				exit(-1);
			}
			//***Dr Murphy's code***//
			else { /* this is the parent -- wait for child to terminate */
				wait(pid1,0,0);
				printf("The parent is exiting now\n");
			}

			printf("\n");

			memset(&myBuff[0],'\0',BUFFER);	//clear myBuff
		}

	}
	while((scantot!=0));
	//while (!feof(stdin));
	printf("\n");	//print new line at end of program

}