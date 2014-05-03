#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

main() {
	int	pid1, pid2;
	int	shmid;
	char	*mysm;
	int	ret_val;

	fprintf(stderr, "Starting the main routine\n");
	if ((shmid = shmget(IPC_PRIVATE,1,0777)) < 0) {
		perror("Could not create shared memory");
		exit(-1);
	}
	else fprintf(stderr, "Shared memory segment created\n");
	if ((pid1 = fork()) == 0){ /* This is the first child */
		fprintf(stderr,"Starting Child 1\n");
		if(( mysm = (char *)shmat(shmid,NULL,0)) < 0) {
			perror("Cannot attach shared memory in p1");
			exit(-1);
		}
		fprintf(stderr, "Child 1 attached shared memory\n");
		sprintf(mysm, "Hello CSC 615!");
		fprintf(stderr, "This is Child 1 exiting\n");
		exit(0);
	}
	fprintf(stderr, "This is the main program again\n");
	if ((pid2 = fork()) == 0) { /* This is the second child */
		fprintf(stderr, "Starting Child 2\n");
		sleep(1);
		fprintf(stderr, "Second Child Waking up\n");
		if ((mysm = (char *)shmat(shmid,NULL,0)) < 0) {
			perror("Cannot attach shared memory in p2");
			exit(-1);
		}
		fprintf(stderr, "Child 2 attached shared memory\n");
		fprintf(stderr, "Message from Child 1:%s\n",mysm);
		fprintf(stderr, "This is Child 2 exiting\n");
		exit(0);
	}
	wait(0);
	wait(0);
	if((ret_val = shmctl(shmid, IPC_RMID,0)) < 0 ) {
		perror("Cannot destroy shared memory segment--do manually!\n");
		exit(-1);
	}
	fprintf(stderr, "This is the main program exiting\n");
}
