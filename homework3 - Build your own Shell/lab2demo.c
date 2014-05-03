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
	
main(int   argc, char *argv[]) {	
	int	pid;

	if ((pid = fork()) == -1 ) { /* error exit - fork failed */ 
		perror("Fork failed");
		exit(-1);
	}
	if (pid == 0) { /* this is the child */
		printf("This is the child ready to execute: %s\n",argv[1]);
		execvp(argv[1],&argv[1]);
		/* error exit - exec returned */
		perror("Exec returned");
		exit(-1);
	} else { /* this is the parent -- wait for child to terminate */
		wait(pid,0,0);
		printf("The parent is exiting now\n");
	}
}