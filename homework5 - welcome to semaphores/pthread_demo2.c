/* to compile:  gcc -o pthread_demo pthread_demo.c -l pthread
 *
*/ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

char	mybuf[25];
pthread_mutex_t my_lock;
sem_t	s_signal;

void first(void	*firstarg ) {
	sleep(1);
	fprintf(stderr, "First thread executing\n");
	pthread_mutex_lock(&my_lock);
	sprintf(&mybuf[0], "Hi CSC 615\n");
	pthread_mutex_unlock(&my_lock);
	sem_post(&s_signal);
}

void second(void *secondarg) {
//	sleep(1);
	fprintf(stderr, "Second thread executing\n");
	sem_wait(&s_signal);
	pthread_mutex_lock(&my_lock);
	fprintf(stderr, &mybuf[0], sizeof(mybuf));
	pthread_mutex_unlock(&my_lock);
}


main() {
	int	retvalue;
	pthread_t tid_first, tid_second;
	void	*arg;

	fprintf(stderr,"Main Program executing\n");

	if ((retvalue = pthread_mutex_init(&my_lock,NULL)) < 0) {
		perror("Can't initialize mutex");
		exit(-1);
	}
	sem_init(&s_signal,0,0);

	if ((retvalue=pthread_create(&tid_first,NULL, (void *)first, arg)) < 0){
		perror("Can't create first thread");
		exit(-1);
	}
	fprintf(stderr,"First Thread Created\n");
	if ((retvalue=pthread_create(&tid_second,NULL, (void *)second, arg)) < 0) exit(-1);
	fprintf(stderr,"Second Thread Created\n");
	fprintf(stderr,"Waiting for first thread to exit\n");
	pthread_join(tid_first,NULL);
	fprintf(stderr,"Waiting for second thread to exit\n");
	pthread_join(tid_second,NULL);
	fprintf(stderr,"Main routine exiting\n");
}
