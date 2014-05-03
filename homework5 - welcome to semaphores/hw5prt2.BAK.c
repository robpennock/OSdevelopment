/* 
 *Robin Pennock
 *hw5prt2.c
 *to compile:  gcc -o hw5prt2 how5prt2.c -lpthread
 *
*/ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>	//recomended from book
#include <sys/types.h>	//for shared memory "shm"
#include <sys/shm.h>

//#include <unistd.h>
//#include <wait.h>


#define MAX_SIZE 10
#define TRUE 1

//global variables
struct timespec tim;
int	ret_val;
int	mutval;
//mutex
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t  semEmpty;	//counting semaphores
sem_t  semFull;	//allows for n threads
int memId;
int randomInt, fullFlag = 0;
int buffer[MAX_SIZE];	
int numItems;
int n, p, c, x, Ptime, Ctime;



void producer(void *firstarg) {
	//mutex p() AKA enter crit section
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	do{	
		//begin p()
		if((sem_wait(&semEmpty))<0){
			perror("Error: P() call failed");
			exit(-1);
		}
		if((mutval = pthread_mutex_lock(&mutex))<0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()
			randomInt = rand();
			

			//do stuff
			
			numItems++;
		//begin V()
		if((mutval = pthread_mutex_unlock(&mutex))<0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}//end v()
		if((sem_post(&semFull))<0){
			perror("Error: P() call failed");
			exit(-1);
		}	//opens u from 
	//exit crit section
	}while(TRUE);	//fix this
}
void consumer(void *firstarg) {
	//mutex p() AKA enter crit section
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	do{	
		//begin p()
		if((sem_wait(&semFull))<0){
			perror("Error: P() call failed");
			exit(-1);
		}
		if((mutval = pthread_mutex_lock(&mutex))<0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()
		
			//do stuff
		
			
		//begin V()
		if((mutval = pthread_mutex_unlock(&mutex))<0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}//end v()
		if((sem_post(&semEmpty))<0){
			perror("Error: V() call failed");
			exit(-1);
		}
	//exit crit section
	}while(TRUE);	//fix this
}
void buffInit(){
	int i;
	for(i=0; i<MAX_SIZE;i++)
		buffer[i]=NULL;	
}
int enqueue(int item){
	if(numItems >= 1)
	if (fullFlag==0)
		buffer[numItems] = item;
	else{
		fullFlag=1;
	}
	
}

main(int argc, char *argv[]) {
	int retval;
	buffInit();
	if(argc != 7){
		perror("Error: argument count is incorrect. Program needs 6");
		exit(-1);
	}
	n=atoi(argv[1]);	//num elements
	p=atoi(argv[2]);	//num producers
	c=atoi(argv[3]);	//num consumers
	x=atoi(argv[4]);	//num items produced per thread
	Ptime=atoi(argv[5]);
	Ctime=atoi(argv[6]);
	
	
	
	
	int	retvalue;	
	//passed in for arg of pthread_create
	void	*arg;		//dont know what type of variable it will be until runtime
	pthread_t prodThreads[p];	//producer thread array
	pthread_t consThreads[c];	//consumer thread array

	fprintf(stderr,"Main Program beginning\n");
	//double check this
	//create shared memory location
	
	/*
	if ((memId = shmget(IPC_PRIVATE,10,IPC_CREAT)) < 0) {
		perror("Could not create shared memory");
		exit(-1);
	}
	if(( buffer = (char *)shmat(memId,NULL,0)) < 0) {
			perror("Cannot attach shared memory in p1");
			exit(-1);
	}*/
	
	//initialize semEmpty
	//not sure about n...?
	
	if((retval = sem_init(&semEmpty,1,n))<0){
		perror("Error initializing EMPTY semaphore");
		exit(-1);
	}
	
	//initialize semEmpty
	//not sure about 0...?
	
	if((retval = sem_init(&semFull,1,(int)0))<0){
		perror("Error initializing EMPTY semaphore");
		exit(-1);
	}

	int i;
	
	for(i=0;i<p;i++){

		//producer thread creation
		if ((retval=pthread_create(&prodThreads[i],NULL, (void *)producer, arg)) < 0){
		perror("Can't create producer threads");
		exit(-1);
		}//return 0 if creation successful	
		
	
	}
	for(i=0;i<c;i++){

		//consumer thread creation
		if ((retval=pthread_create(&consThreads[i],NULL, (void *)consumer, arg)) < 0){
		perror("Can't create consumer threads");
		exit(-1);
		}//return 0 if creation successful	
		
	
	}
	for(i=0;i<p;i++){
		if(pthread_join(prodThreads[i],NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	for(i=0;i<c;i++){
		if(pthread_join(consThreads[i],NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	if((pthread_mutex_destroy(&mutex))<0){
		perror("Error killing mutex");
		exit(-1);
	}
}