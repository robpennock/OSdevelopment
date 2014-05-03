#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <errno.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/stat.h>


main() {
	int	pid1, pid2;
	int	shmid;
	char	*mysm;
	int	ret_val;
	int	mutex;
	int	s_signal;
	int	semval;
	struct	sembuf	sem_P;	//p calls
	struct	sembuf	sem_V;	//v calls
	int	key;

	fprintf(stderr, "Starting the main routine\n");
	
	sem_P.sem_num=0;	
	sem_P.sem_op=-1;
	sem_P.sem_flg=0;

	sem_V.sem_num=0;
	sem_V.sem_op=1;
	sem_V.sem_flg=0;
	key=time(0);

	if ((shmid = shmget(IPC_PRIVATE,1,0777|IPC_CREAT)) < 0) {
		perror("Could not create shared memory");
		exit(-1);
	}
	fprintf(stderr, "Shared memory segment created\n");

	if ((mutex = semget(key,1,0777|IPC_CREAT)) < 0) {
		perror("Could not create mutex semaphore");
		exit(-1);
	}
	if ((s_signal = semget(key+1,1,0777|IPC_CREAT)) < 0) {
		perror("Could not create signal semaphore");
		exit(-1);
	}
	fprintf(stderr, "Semaphores created\n");
		
	if((ret_val = semop(mutex,&sem_V,1)) < 0) {
		perror("Can't do a V call in main\n");
		exit(-1);
	}
	fprintf(stderr,"Mutex semaphore initialized\n");
	
//work on first process
	if ((pid1 = fork()) == 0){ /* This is the first child */
		fprintf(stderr,"Starting Child 1\n");
		sleep(1);
		if(( mysm = (char *)shmat(shmid,NULL,0)) < 0) {
			perror("Cannot attach shared memory in p1");
			exit(-1);
		}

		fprintf(stderr, "Child 1 attached shared memory\n");
		fprintf(stderr, "Child 1 is getting mutex and updating shared  memory\n");
		if((ret_val = semop(mutex,&sem_P,1)) <0) {
			perror("Can't do a P call in process 1\n");
			exit(-1);
		}
		sprintf(mysm, "Hello CSC 615!");
		if((ret_val = semop(mutex,&sem_V,1)) <0) {
			perror("Can't do a V call in process 1\n");
			exit(-1);
		}
		fprintf(stderr, "Child 1 wrote to shared memory and is calling V(s_signal)\n");
		if((ret_val = semop(s_signal,&sem_V,1)) < 0) {
			perror("Can't do a V call in process 1\n");
			exit(-1);
		}
		fprintf(stderr, "This is Child 1 exiting\n");
		exit(0);
	}
	//second process
	fprintf(stderr, "This is the main program again\n");
	if ((pid2 = fork()) == 0) { /* This is the second child */
		fprintf(stderr, "Starting Child 2 and calling P(v_signal)\n");
		if((ret_val = semop(s_signal,&sem_P,1)) < 0) {
			perror("Can't do a P call in process 2\n");
			exit(-1);
		}
		fprintf(stderr, "Second Child Waking up\n");
		if ((mysm = (char *)shmat(shmid,NULL,0)) < 0) {
			perror("Cannot attach shared memory in p2");
			exit(-1);
		}
		fprintf(stderr, "Child 2 attached shared memory\n");
		fprintf(stderr,"Child 2 getting mutex and accessing shared memory\n");
		if((ret_val = semop(mutex,&sem_P,1)) <0) {
			perror("Can't do a P call in process 2\n");
			exit(-1);
		}
		fprintf(stderr, "Message from Child 1: %s\n",mysm);
		if((ret_val = semop(mutex,&sem_V,1)) <0) {
			perror("Can't do a V call in process 2\n");
			exit(-1);
		}
		fprintf(stderr, "This is Child 2 exiting\n");
		exit(0);
	}
	wait(0);
	wait(0
	//kill off semaphores
	if((ret_val = shmctl(shmid, IPC_RMID,0)) < 0 ) {
		perror("Cannot destroy shared memory segment--do manually!\n");
		exit(-1);
	}
	if((ret_val = semctl(mutex,IPC_RMID,0)) < 0) {
		perror("Cannot destroy mutex semaphore -- do manually!\n");
		exit(-1);
	}
	if((ret_val = semctl(s_signal,IPC_RMID,0)) < 0) {
		perror("Cannot destroy signal semaphore -- do manually!\n");
		exit(-1);
	}
	fprintf(stderr, "This is the main program exiting\n");
}
