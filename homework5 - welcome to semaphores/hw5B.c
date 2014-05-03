/*hw5B.c
	by Robin Pennock
	12 April 2013
*/
/*
	This program demonstrates the use of counting semaphores

*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 7
#define TRUE 1

pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t  semEmpty;	//counting semaphores
sem_t  semFull;	//allows for n threads

static int in = 0;
static int out = 0;
static int globA=0, globB=0;
static int *buffer, *prodOutbBuff, *consOutBuff;
int	n, p, c, x, Ptime, Ctime;
struct timespec tim;
//producer struct
struct	producer{
	pthread_t threadId;
	int threadNum;
	int locX;
};
//consumer struct
struct	consumer{
	pthread_t threadId;
	int threadNum;
	int locX;
};
void timestamp()
//http://stackoverflow.com/questions/9596945/how-to-get-appropriate-timestamp-in-c-for-logs
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    fprintf(stderr,"%s",asctime( localtime(&ltime) ) );
}

void enqueue(void *firstarg){
//begin p()
	
	int	semval;
	int nextProduced;
	struct producer *temp = firstarg;	//redeclaration
	tim.tv_sec = Ptime/CLOCKS_PER_SEC;
	//100million nanoseconds is 1/10 second
    tim.tv_nsec = 0;
	while(temp->locX>0){
		//p()
		if((sem_wait(&semEmpty))<0){
			perror("Error: P() call failed in producer");
			exit(-1);
		}
		if((semval = pthread_mutex_lock(&mutex))!=0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()
		nextProduced = globA;
		buffer[in] = nextProduced;
		prodOutbBuff[globA] = nextProduced;	//comparison array
		globA++;
		//fprintf(stderr, "Thread %ld produced %d", (long)temp->threadId, nextProduced, temp->threadNum);
		fprintf(stderr, "Thread %ld produced %d\n", (long)temp->threadId, nextProduced);
		in = (in + 1) % n;
		temp->locX--;
		//v()
		if((semval = pthread_mutex_unlock(&mutex))!=0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}
		if((sem_post(&semFull))<0){
			perror("Error: V() call failed in producer");
			exit(-1);
		}	//end v()
	//exit crit section
		nanosleep(&tim , NULL);
	}
	
}
void dequeue(void *firstarg){
	int	semval;
	//int nextConsumed;
	struct consumer *temp = firstarg;
	tim.tv_sec = Ctime/CLOCKS_PER_SEC;
	//100million nanoseconds is 1/10 second
    tim.tv_nsec = 0;
	
	//fprintf(stderr, "Consumer %ld running \n", (long)temp->threadId);
	while(temp->locX>0){
		//p()
		if((sem_wait(&semFull))<0){
			perror("Error: P() call failed in consumer");
			exit(-1);
		}
		if((semval = pthread_mutex_lock(&mutex))!=0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()
		int nextConsumed = buffer[out];	//read 
		consOutBuff[globB] = nextConsumed;	//comparison array
		globB++;	
		out = (out + 1) % n;
		temp->locX--;
		
		//begin V()
		if((semval = pthread_mutex_unlock(&mutex))!=0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}
		if((sem_post(&semEmpty))<0){
			perror("Error: V() call failed in consumer");
			exit(-1);
		}//end v()
		fprintf(stderr, "Num %d consumed by consumer %ld\n", nextConsumed, (long)temp->threadId);
		
		nanosleep(&tim , NULL);
	}
}

main(int argc, char *argv[]) {
	int retval;
	int i;
	struct producer *producer_mem;
	struct consumer *consumer_mem;
	int totTimeMin=0, totTimeSec=0;
	time_t start, stop;
	time(&start);
	
	if(argc != 7){
		perror("Error: argument count is incorrect. Program needs 6");
		exit(-1);
	}
	n=atoi(argv[1]);	//num elements
	p=atoi(argv[2]);	//num producers
	c=atoi(argv[3]);	//num consumers
	x=atoi(argv[4]);	//num items produced per thread
	Ptime=atoi(argv[5]);	//producer spin wait time
	Ctime=atoi(argv[6]);	//consumer spin wait time
		
	//memory allocation
	buffer =  calloc (n, sizeof(int));
	prodOutbBuff =  calloc ((p*x), sizeof(int));
	consOutBuff =  calloc ((p*x), sizeof(int));
	producer_mem =  calloc (p, sizeof(struct producer));
	consumer_mem =  calloc (c, sizeof(struct consumer));
	
	timestamp();
	//initialize semEmpty to n
	if((retval = sem_init(&semEmpty,1,n))<0){
		perror("Error initializing EMPTY semaphore");
		exit(-1);
	}
	
	//initialize semEmpty to 0
	if((retval = sem_init(&semFull,1,(int)0))<0){
		perror("Error initializing FULL semaphore");
		exit(-1);
	}
	//passed in for arg of pthread_create
	//void	*arg;		//dont know what type of variable it will be until runtime
	for(i=0;i<p;i++){
	//producer thread creation
	producer_mem[i].threadNum = p;
	producer_mem[i].locX = x;
	if ((retval=pthread_create(&producer_mem[i].threadId,NULL, (void*)enqueue, &producer_mem[i])) < 0){
		perror("Can't create producer threads");
		exit(-1);
		}//return 0 if creation successful	
		
	}
	for(i=0;i<c;i++){
		consumer_mem[i].threadNum = c;
		consumer_mem[i].locX = p*x/c;
		//consumer thread creation
		if ((retval=pthread_create(&consumer_mem[i].threadId,NULL, (void *)dequeue, &consumer_mem[i])) < 0){
		perror("Can't create consumer threads");
		exit(-1);
		}//return 0 if creation successful	
		
	
	}
	for(i=0;i<p;i++){
		if(pthread_join(producer_mem[i].threadId,NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	for(i=0;i<c;i++){
		if(pthread_join(consumer_mem[i].threadId,NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	//http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzahw%2Frzahwe18rx.htm
	if((sem_destroy(&semEmpty))<0){
		perror("Error killing emptySemaphore");
		exit(-1);
	}
	if((sem_destroy(&semFull))<0){
		perror("Error killing fullSemaphore");
		exit(-1);
	}
	if((pthread_mutex_destroy(&mutex))!=0){
		perror("Error killing mutex");
		exit(-1);
	}
	for(i=0; i<globA; i++)
		fprintf(stderr,"%d ", prodOutbBuff[i]);
	fprintf(stderr,"\n Total produced = %d\n",globA);	
	for(i=0; i<globB; i++)
		fprintf(stderr,"%d ", consOutBuff[i]);
	fprintf(stderr,"\n Total consumed = %d\n",globB);
	time(&stop);
	fprintf(stderr,"Total Time elapsed: %0.2f seconds \n", difftime(stop, start));
	timestamp();
	fprintf(stderr,"Main Program Finishing\n");
	
}
	
