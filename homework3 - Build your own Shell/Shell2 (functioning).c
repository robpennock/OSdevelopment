/* Shell2 By Robin Pennock
**
** 	
**	much of the basic process control modified from Dr Murpheys hw3 examples
**	http://dbsystems.sfsu.edu/~csc415/Assignments/Demo%20Code/HW3/Linux/615_lab2demo2.c
**	accessed 28 Feb 2013
*/

/*
	This is nearly a fully functional shell
	Shell can now handle pipes and redirects
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
//***needed for exit() ***///
#include <stdlib.h>
//***for memset()***//
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER 256
#define SHELLOUT "Shell2: > "



main(int   argc, char *argv[]) {
	int	pid1, pid2, scantot;
	char end;
	int fd1;
	char myBuff[BUFFER];
	int argsize;
	char * tokPtr;
	int	pipeID[2];
	int	status;
	int writeFileDescriptor;
	//flags
	int redirFlag=0 , appendFlag=0 , fromFile=0 , pipeFlag=0;
	int pipeIndex;
	char * leftargv[128], *rightargv[128];
	int i=0, j=0, leftargc=0, rightargc=0;

	///***begin loop***//
	do
	{
		memset(&myBuff[0],'\0', BUFFER);	//clear myBuff
		//***make sure to set flags to false for each iteration***//
		redirFlag=0;
		appendFlag=0;
		fromFile=0;
		pipeFlag=0;
		//***read and tokenize input commands and place into argv***//
		write(1, SHELLOUT, sizeof(SHELLOUT));		//need to check read and write for errors
		scantot = read(0, &myBuff[0], BUFFER);
		//printf("%s", myBuff);		//for error checking
		if(scantot==0)
			break;
		//***if carriage returned entered by itself***//
		if(myBuff[0]==10)	//not sure if this is good coding but it works...
			continue;
		if(strcmp(myBuff, "exit")==0)	//exit if exit command given
			exit(0);
		else{
			//echoSize = sprintf(&echo[0], "You have entered %s", &myBuff[0]);
			//write(1, &echo, echoSize);
			argsize = 0;
			//***tokenize buffer with delimiters " \n\0"  ***//
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
		//*********pipes****can only handle a single pipe right now
		for(i=0;i<argsize;i++){
			if(strcmp(argv[i], "|")==0){
				pipeFlag=1;		//pipe found
				pipeIndex=i;
				leftargc = i;
				rightargc = argsize-i-1;	//be sure to use argsize
				break;
			}
		}
		if(pipeFlag==1){
			//printf("\nPipe found\n");
			//populate leftargv[]
			for(i=0; i<leftargc;i++){
				leftargv[i]=argv[i];
			}
			leftargv[i]=NULL;	//make sure to make last arg null
			/*	//debuging leftargv
			for(i=0; i<leftargc; i++)
				printf("\nleftargv[%d] = %s\n",i,leftargv[i]);
			*/
			for(i=pipeIndex+1, j=0; i<argsize; i++, j++){
				rightargv[j]=argv[i];	//populat rightargv[]
			} 
			rightargv[i]=NULL;
			/*	//debugging rightargv
			for(i=0; i<rightargc; i++)
				printf("\nrightargv[%d] = %s\n",i,rightargv[i]);
			
			*/
		
			//beginning pipe process stuff
			/****Entire block up until continue statement 
				on line 152 modified from Dr Murphey's demo code 
			*/
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
				//printf("The first child is starting: %s\n",rightargv[0]);
				close (0);		//close stdin
				dup (pipeID[0]);	//link pipe[0] to stdin
				close (pipeID[0]);	//close pipes
				close(pipeID[1]);
				execvp(rightargv[0],&rightargv[0]); 	//exec right side args
				/* error exit - exec returned */
				perror("Execvp returned");		//if error
				exit(-1);
			}
		/* this is the parent again */
			if ((pid2 = fork()) == -1) { /* error exit - bad fork */
				perror("Bad fork");
				exit(-1);
			}
			if (pid2 == 0) { /* the second child process */
				//printf("The second child is starting: %s \n",leftargv[0]);
				close (1);	//close stdout	
				dup(pipeID[1]);		//link pipeID[1] to stdout
				close(pipeID[0]);close(pipeID[1]);	//close pipes
				execvp(leftargv[0],&leftargv[0]); 	//execute left side args
				/* error exit - exec returned */
				perror("Execl returned");		//if error
				exit(-1);
			}
			/* back to the parent again */
			close(pipeID[0]);		//close pipes
			close(pipeID[1]);
			wait(pid1,0,0);			//close processes
			wait(pid2,0,0);
			//printf("The parent is exiting\n");
			memset(leftargv, '\0', sizeof(leftargv));		//clear leftargv and rightargv
			memset(leftargv, '\0', sizeof(rightargv));
					
			continue;	//keeps from running last command again
		}	//end piping
		
		//***if '&' found***//
		//argsize -1 or else segmentation fault
		if((strcmp(argv[argsize-1], "&")==0) && (argsize > 1)){
			//printf("& found. running process in background \n");
			argv[argsize-1] = NULL;	//remove & from argv
			argsize--;	//optional
			//***program itself is parent***//
			//***clone process except for pid***//
			if ((pid2 = fork()) == -1 ) { /* error exit - fork failed */
				perror("Fork failed");
				exit(-1);
			}
			if (pid2 == 0) { /* this is the child */
				//printf("This is the child ready to execute: %s\n",argv[0]);
				execvp(argv[0],&argv[0]);		//execute all args before &
				/* error exit - exec returned */
				perror("Exec returned");
				exit(-1);
			}
			//these are necessary
			int i;
			for(i=0;i<argc;i++)	//clear argv
				argv[i]=NULL;
			//***wait with WNOHANG crucial for running in background***//
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
			//write only; append; create if necessary
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
		//***these share similar code ***//
		//*** the only thing that changes between different redirects is stdin/stdout locations
		if(redirFlag||appendFlag||fromFile){
					
			//remove remaining elements
			argv[argsize-2]=NULL;
			argv[argsize-1]=NULL;
			//***from Dr Murphey's demo code on hw3***//
			if ((pid1 = fork()) == -1) {  /* error exit - bad fork */
				perror("Bad fork");
				exit(-1);
			}
			//***from Dr Murphey's demo code on hw3***//
			if (pid1 == 0) { /* The first child process */
				/* make sure you start the last process in the pipeline first! */
				//printf("The first child is starting: %s\n",argv[0]);
				//***only need to redirect stdout to file
				if(redirFlag||appendFlag)
					close (1);	//close stdout
				else if(fromFile)
					close(0);
				dup (writeFileDescriptor);	//dup file descriptor to stdout
				close (writeFileDescriptor);	//make sure to close file descriptor
				//close(pipeID[1]);
				//***use execvp, grapbs all args; no need for null***//
				execvp(argv[0],&argv[0]); 
				/* error exit - exec returned */
				perror("Execl returned");
				exit(-1);
			}
			/* back to the parent again */
			wait(pid1,0,0);
			
			//printf("The parent is exiting\n");
			redirFlag=0;
			appendFlag=0;
		
		}
		
		//default; if all special cases not found, just execute the single command
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
				//printf("This is the child ready to execute: %s\n",argv[0]);
				execvp(argv[0],&argv[0]);
				/* error exit - exec returned */
				perror("Exec returned");
				exit(-1);
			}
			//***Dr Murphy's code***//
			else { /* this is the parent -- wait for child to terminate */
				wait(pid1,0,0);
				//printf("The parent is exiting now\n");
			}

			printf("\n");

			
		}
		memset(&myBuff[0],'\0',BUFFER);	//clear myBuff
	}
	while((scantot!=0));	//while cntrl-d not entered
	if(pid2>0){		//kill off last child process
		wait(pid2,0,0);
		printf("\nClosing background child");
	}

	printf("\n");	//print new line at end of program

}