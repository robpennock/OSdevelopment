/* to compile:  gcc -o pthread_demo pthread_demo.c -lpthread
 *
*/ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

char	mybuf[25];

void first(void	*firstarg ) {
	fprintf(stderr, "First thread executing\n");
	sprintf(&mybuf[0], "Hi CSC 615");
}

void second(void *secondarg) {
	fprintf(stderr, "Second thread executing\n");
	sleep(1);
	fprintf(stderr,"buffer output: ");
	fprintf(stderr, &mybuf[0], sizeof(mybuf));
	fprintf(stderr,"\n");
}


main() {
	int	retvalue;
	pthread_t tid_first, tid_second;	//pthread IDs	
	void	*arg;		//dont know what type of variable it will be until runtime

	fprintf(stderr,"Main Program executing\n");
	//create pthread
	if ((retvalue=pthread_create(&tid_first,NULL, (void *)first, arg)) < 0){
		perror("Can't create first thread");
		exit(-1);
	}
	fprintf(stderr,"First Thread Created\n");
	//create second pthread
	//typecast second function output to (void *)
	if ((retvalue=pthread_create(&tid_second,NULL, (void *)second, arg)) < 0) exit(-1);

	fprintf(stderr,"Second Thread Created\n");
	fprintf(stderr,"Waiting for first thread to exit\n");
	pthread_join(tid_first,NULL);	//wait for threads to terminate
	fprintf(stderr,"Waiting for second thread to exit\n");
	pthread_join(tid_second,NULL);
	fprintf(stderr,"Main routine exiting\n");
}