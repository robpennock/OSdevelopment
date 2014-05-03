/* 
 *Robin Pennock
 *homework4.c
 *to compile:  gcc -o homework4 homework4.c -lpthread
 *
*/ 

/*
	in this program, 10 threads all add 10 to a global varable (intially 0)
	There is no mutex lock, the final variable assignment should not increment the global.
	So the final value of the global should be 0.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10

//global variables
int x=0, i=0;
struct timespec tim;

void run(void *firstarg) {
	i++;
	int locI=i;
	tim.tv_sec = 1;
	//500million nanoseconds is 1/2 second
    tim.tv_nsec = 500000000;
	//make sure to define x1 within function body DO NOT MAKE GLOBAL
	int x1=x;
	fprintf(stderr, "Hello I'm thread #%ld(%d): local x=%d\n", (long)pthread_self(),locI, x1);
	x1+=10;	//add 10 to local x
	//make sure to static cast the return from pthread_create() to the type you want for printing
	fprintf(stderr, "\t Finishing thread #%ld(%d): local x after addition=%d\n", (long)pthread_self(), locI , x1);
	nanosleep(&tim , NULL);
	x=x1;	//increment global variable
	
}

main() {
	int	retvalue;	
	//passed in for arg of pthread_create
	void	*arg;		//dont know what type of variable it will be until runtime
	pthread_t threads[SIZE];	//pthread array
	fprintf(stderr,"Main Program beginning\n");
	int i;
	for(i=0;i<SIZE;i++){
		if ((retvalue=pthread_create(&threads[i],NULL, (void *)run, arg)) < 0){
		perror("Can't create first thread");
		exit(-1);
		}//return 0 if creation successful	
	}
	for(i=0;i<SIZE;i++){
		if(pthread_join(threads[i],NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	fprintf(stderr, "Global x=%d\n", x);
}