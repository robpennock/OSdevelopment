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

//int nextProduced = 10;

int	n, p, c, x, Ptime, Ctime;
struct timespec tim;
struct tm timeStruct;
struct	producer{
	pthread_t threadId;
	int threadNum;
	int locX;
};
struct	consumer{
	pthread_t threadId;
	int threadNum;
	int locX;
};
//http://stackoverflow.com/questions/9596945/how-to-get-appropriate-timestamp-in-c-for-logs
void timestamp()
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
	while(temp->locX>0){/*
		
		while(temp->locX==sizeof(buffer))
			;	//wait*/
		/*
		if((sem_wait(&semEmpty))<0){
			perror("Error: P() call failed in producer");
			exit(-1);
		}
		if((semval = pthread_mutex_lock(&mutex))!=0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()*/
		nextProduced = globA;
		buffer[in] = nextProduced;
		prodOutbBuff[globA] = nextProduced;	//comparison array
		globA++;
		//fprintf(stderr, "Thread %ld produced %d", (long)temp->threadId, nextProduced, temp->threadNum);
		fprintf(stderr, "Thread %ld produced %d\n", (long)temp->threadId, nextProduced);
		in = (in + 1) % n;
		temp->locX--;
		/*
		if((semval = pthread_mutex_unlock(&mutex))!=0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}//end v()
		if((sem_post(&semFull))<0){
			perror("Error: V() call failed in producer");
			exit(-1);
		}	//opens u from */
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
		/*
		if((sem_wait(&semFull))<0){
			perror("Error: P() call failed in consumer");
			exit(-1);
		}
		if((semval = pthread_mutex_lock(&mutex))!=0){
			perror("\nMutex lock failed\n");
			exit(-1);
		}//end p()*/
		/*
		while(temp->locX==0)
			;	//wait*/
		/*
		int z;
		for(z=0;z<n; z++)
			fprintf(stderr, "BUFFER[%d] = %d\n",z,buffer[z]);
		fprintf(stderr, "after consumer accesses buffer\n");*/
		int nextConsumed = buffer[out];
		consOutBuff[globB] = nextConsumed;	//comparison array
		globB++;	
		out = (out + 1) % n;
		temp->locX--;
		
		//begin V()
		/*
		if((semval = pthread_mutex_unlock(&mutex))!=0){
			perror("\nMutex unlock failed\n");
			exit(-1);
		}//end v()
		if((sem_post(&semEmpty))<0){
			perror("Error: V() call failed in consumer");
			exit(-1);
		}*/
		fprintf(stderr, "Num %d consumed by consumer %ld\n", nextConsumed, (long)temp->threadId);
		
		nanosleep(&tim , NULL);
	}
}

main(int argc, char *argv[]) {
	int retval;
	int i;
	struct producer *producer_mem;
	struct consumer *consumer_mem;
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
	Ptime=atoi(argv[5]);
	Ctime=atoi(argv[6]);
		
	//memory allocation
	buffer =  calloc (n, sizeof(int));
	prodOutbBuff =  calloc ((p*x), sizeof(int));
	consOutBuff =  calloc ((p*x), sizeof(int));
	producer_mem =  calloc (p, sizeof(struct producer));
	consumer_mem =  calloc (c, sizeof(struct consumer));
	timestamp();
	
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
	//passed in for arg of pthread_create
	//void	*arg;		//dont know what type of variable it will be until runtime
	//pthread_t prodThreads[p];	//producer thread array
	//pthread_t consThreads[c];	//consumer thread array
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
	}//destroy semEmpty
	if((sem_destroy(&semFull))<0){
		perror("Error killing fullSemaphore");
		exit(-1);
	}//destroy semFull
	if((pthread_mutex_destroy(&mutex))!=0){
		perror("Error killing mutex");
		exit(-1);
	}//destoy mutex
	//print checking buffers
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
	
