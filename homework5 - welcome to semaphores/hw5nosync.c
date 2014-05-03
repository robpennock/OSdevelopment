/* 
 *Robin Pennock
 *hw5nosync.c
 *to compile:  gcc -o hw5nosync hw5nosync.c -lpthread
 *
*/ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>	//recomended from book


#define SIZE 10

//global variables
int x=0, i=0;
struct timespec tim;
int	ret_val;
int	semval;
//mutex
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;



void run(void *firstarg) {
	//mutex p() AKA enter crit section
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	if((semval = pthread_mutex_lock(&mutex))<0){
		perror("\nMutex lock failed\n");
		exit(-1);
	}
	i++;
	int locI=i;
	tim.tv_sec = 0;
	//100million nanoseconds is 1/10 second
    tim.tv_nsec = 100000000;
	//make sure to define x1 within function body DO NOT MAKE GLOBAL
	int x1=x;
	fprintf(stderr, "Hello I'm thread #%ld(%d): local x=%d\n", (long)pthread_self(),locI, x1);
	x1+=10;	//add 10 to local x
	//make sure to static cast the return from pthread_create() to the type you want for printing
	fprintf(stderr, "\t Finishing thread #%ld(%d): local x after addition=%d\n", (long)pthread_self(), locI , x1);
	nanosleep(&tim , NULL);
	x=x1;	//increment global variable
	//mutex v()
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	/*if((semval = pthread_mutex_unlock(&mutex))<0){
		perror("\nMutex unlock failed\n");
		exit(-1);
	}*/
	//exit crit section
}

main() {
	int	retvalue;	
	//passed in for arg of pthread_create
	void	*arg;		//dont know what type of variable it will be until runtime
	pthread_t threads[SIZE];	//pthread array

	fprintf(stderr,"Main Program beginning\n");

	int i;
	for(i=0;i<SIZE;i++){

		//thread creation
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
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	if((pthread_mutex_destroy(&mutex))<0){
		perror("Error killing mutex");
		exit(-1);
	}
	fprintf(stderr, "Global x=%d\n", x);
}