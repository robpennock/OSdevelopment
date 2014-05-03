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
#include <sys/stat.h>
#include <fcntl.h>
//***for uniform buffer size***//
#define BUFFER 256
#define SHELLOUT "Shell1: > "



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
	char writeBuffer[4096];
	int openFileDescriptor;
	int writeFileDescriptor;
	int redirFlag=0;
	int appendFlag=0;
	int fromFile=0;
	int pipeFlag=0;
	int pipeIndex;
	char * leftargv[128], *rightargv[128];

	//argsize = argc;
	
	
	///***begin loop***//
	do
	{
		memset(&myBuff[0],'\0', BUFFER);	//clear myBuff
		//***make sure to set flags to false***//
		redirFlag=0;
		appendFlag=0;
		fromFile=0;
		//***read and tokenize input commands and place into argv***//
		write(1, SHELLOUT, sizeof(SHELLOUT));		//need to check read and write for errors
		scantot = read(0, &myBuff[0], BUFFER);
		if(scantot==0)
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
				//printf("%s\n",argv[argsize]);
				argsize++;
			}

			argv[argsize] = NULL;	//make sure final argv is null
		}
		
		//***if '&' found***//
		//argsize -1 or else segmentation fault
		if((strcmp(argv[argsize-1], "&")==0) && (argsize > 1)){
			printf("& found. running process in background \n");
			argv[argsize-1] = NULL;	//remove & from argv
			argsize--;	//optional
			//***program itself is parent***//
			//***clone process except for pid***//
			if ((pid2 = fork()) == -1 ) { /* error exit - fork failed */
				perror("Fork failed");
				exit(-1);
			}
			if (pid2 == 0) { /* this is the child */
				printf("This is the child ready to execute: %s\n",argv[0]);
				//execvp(argv[0],&argv[0]);
				
				execvp(argv[0],&argv[0]);
				/* error exit - exec returned */
				perror("Exec returned");
				exit(-1);
			}
			//these are necessary
			int i;
			for(i=0;i<argc;i++)	//clear argv
				argv[i]=NULL;
			wait(pid2, NULL, WNOHANG);	//dont wait for process to finish executing
			continue;

			}
		//***if > set redirFlag and open file descriptor***//
		else if((argsize>2)&&(strcmp(argv[argsize-2], ">")==0)){
			redirFlag=1;
			//write only; create file, truncate to overwrite preivous text
			if ((writeFileDescriptor = open(argv[argsize-1],O_WRONLY|O_CREAT|O_TRUNC))<0){
				perror("File open failed");
				exit (-1);
				}
		}
		//if >>  set appendFlag and open file descriptor***//
		else if((argsize>2)&&(strcmp(argv[argsize-2], ">>")==0)){
			appendFlag=1;
			//write only; append; creat if necessary
			if ((writeFileDescriptor = open(argv[argsize-1],O_WRONLY|O_APPEND|O_CREAT))<0){
				perror("File open failed");
				exit (-1);
				}
		}
		//***redirecting stdout from file***//
		else if((argsize>2)&&(strcmp(argv[argsize-2], "<")==0)){
			fromFile=1;
			if ((writeFileDescriptor = open(argv[argsize-1],O_RDONLY))<0){
				perror("File open failed");
				exit (-1);
				}
		}
		if(redirFlag||appendFlag||fromFile){
					
			//remove remaining elements
			argv[argsize-2]=NULL;
			argv[argsize-1]=NULL;
			
			if ((pid1 = fork()) == -1) {  /* error exit - bad fork */
				perror("Bad fork");
				exit(-1);
			}
			if (pid1 == 0) { /* The first child process */
				/* make sure you start the last process in the pipeline first! */
				printf("The first child is starting: %s\n",argv[0]);
				if(redirFlag||appendFlag)
					close (1);	//close stdout
				else if(fromFile)
					close(0);
				dup (writeFileDescriptor);	//dup file descriptor to stdout
				close (writeFileDescriptor);
				//close(pipeID[1]);
				//***use execvp, grapbs all args; no need for null***//
				execvp(argv[0],&argv[0]); 
				/* error exit - exec returned */
				perror("Execl returned");
				exit(-1);
			}
			/* back to the parent again */
			wait(pid1,0,0);
			
			printf("The parent is exiting\n");
			redirFlag=0;
			appendFlag=0;
		
		}
		//*********pipes
		int i;
		for(i=0;i<argsize;i++){
			if(strcmp(argv[i], "|")==0)
				pipeFlag=1;
				pipeIndex=i;
		}
		if(pipeFlag==1){
			
			
		}
		//default
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

			
		}
		memset(&myBuff[0],'\0',BUFFER);	//clear myBuff
	}
	while((scantot!=0));
	if(pid2>0){
		wait(pid2,0,0);
		printf("\nClosing background child");
	}
	//wait(pid2,0,0);
	//while (!feof(stdin));
	printf("\n");	//print new line at end of program

}