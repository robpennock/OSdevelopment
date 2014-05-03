/* 
 *Robin Pennock
 *homework8.c
 *to compile:  gcc -o h6 hw6.c -lpthread -lm
 *
*/ 

/*
	This program demonstrates the efficiency of various sorting algorithms
		using threads

	Program takes in P and N from user
		-P is the size of the arrays, populated with random numbers
		-N is the number of threads to used to break the work up
		
	Program first runs squential search for baseline comparison
	
	Second it runs quicksort broken up into N parts (threads) and are sorted
	
	Time is kept for comparisons
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int * startArray;
int * minArray;
int * sortedArray;
int * randArray;
int * finalArray;
struct threadStruct *thread_mem;
int inP;
int inN;
int p;
int n;

//basic struct needed for thread info
struct	threadStruct{
	pthread_t threadId;		//pthread in linux
	int threadIndex;
	int localMin;
	int counter;
};
//start function for created threads
void run(void * arg){
	struct threadStruct * temp = arg;
	int range = p/n;
	int start = (temp->threadIndex) * range;
	int end = start + range;
	temp->counter = range;
	//temp->localMin = findMin(startArray,start,end);
	sort(sortedArray, start, end);
	minArray[temp->threadIndex] = start;	//min array using current thread as index
}
//insertion sort
void sort (int * array, int first, int last)
{	//super easy quicksort from data structures
    int i;
	int j;
	int temp;
    for (i = first; i < last; i++) {
        
        for (j = i; j < last; j++) {
            if (array[i] > array[j]) {
                // Swap the contents arrays.
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}
int	getNext(){
	int	smallest;
	int minval; 
	int i;
	//fprintf(stderr,"getNext running\n");
	minval=0;
	for(i=0;i<n; i++){
		if(sortedArray[minArray[i]]<sortedArray[minArray[minval]])	{
			minval = i;	
		}
	}
	smallest=sortedArray[minArray[minval]];
	if((thread_mem[minval].counter)>1){
		minArray[minval]++;
		thread_mem[minval].counter--;
		//fprintf(stderr,"Second if done \n");
	}
	else{
		sortedArray[minArray[minval]]=RAND_MAX;
	}
	
	//fprintf(stderr,"min val: %d\n", smallest);
	return(smallest);
}

void execute(){
	int retval;
	int i;
	int threadNum=0;
	int finalMin;
	clock_t start;
	clock_t finish;
	double totalTime=0;
	
	
	//allocating dynamic arrays and thread array
	minArray = calloc(16, sizeof(int));
	sortedArray =  calloc (p, sizeof(int));
	finalArray =  calloc (p, sizeof(int));
	thread_mem =  calloc (16, sizeof(struct threadStruct));
	//populate arrays
	fprintf(stderr, "Populating arrays\n");
	
	//fprintf(stderr, "randArray populated\n");
	for(i=0;i<p; i++){
		sortedArray[i]=startArray[i];
	}
	
	
	fprintf(stderr, "Begin thread creation\n");
	//http://www.codecogs.com/reference/computing/c/time.h/clock.php
	start = clock();	//begin clocking
	for(i=0;i<n;i++){
	//fprintf(stderr, "checking for errors at beginning\n");
	thread_mem[i].threadIndex = i;
	if ((retval=pthread_create(&thread_mem[i].threadId,NULL, (void*)run, &thread_mem[i])) < 0){
		perror("Can't create producer threads");
		exit(-1);
		}//return 0 if creation successful	
	}
	for(i=0;i<n;i++){
		if(pthread_join(thread_mem[i].threadId,NULL)){
		perror("Error exiting thread");
		exit(-1);
		}//return 0 if creation successful
	}
	fprintf(stderr, "Threads done working\n");
	//printArray(minArray, n);
	for(i=0; i<p; i++){
		finalArray[i]=getNext();
		//fprintf(stderr, "getNext run: %d\n", i);
	}
	
	finish = clock();	//end clocking
	totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
	
	fprintf(stderr, "Total Time spent with concurent sort was: %.26f\n\n", totalTime);
}

void printArray(int * anArray, int top){
	int i;
	for(i=0; i<top; i++)
		fprintf(stderr,"array[%d] = %d\n", i, anArray[i]);
}

main(int   argc, char *argv[]) {
	
	int i;
	clock_t start;
	clock_t finish;
	double totalTime=0;
	
	if(argc != 2){
		perror("Error: Program must have 1 arguments\n");
		exit(-1);
	}
	p=atoi(argv[1]);
	n=0;
	//raise to power
	p = (int)pow(2.0, p);
	n = (int)pow(2.0, n);
	
	startArray =  calloc (p, sizeof(int));
	randArray = calloc (p, sizeof(int));
	
	//populate initial random arrays
	for(i=0;i<p; i++){
		startArray[i]=rand()%RAND_MAX;
	}
	fprintf(stderr, "startArray populated\n");
	for(i=0;i<p; i++){
		randArray[i]=startArray[i];
	}	
	
	fprintf(stderr, "Sequentially sorting array\n");
	start = clock();
	//sequential sort
	sort(randArray,0,p);
	finish = clock();	//end clocking
	totalTime = (double)(finish - start) / CLOCKS_PER_SEC;
	fprintf(stderr, "Total Time spent for sequential sort was: %.26f\n\n", totalTime);
	
	fprintf(stderr, "P = %d, N = %d\n", p ,n);
	//execute for different number of threads
	execute(p, n);
	n=1;
	n = (int)pow(2.0, n);
	fprintf(stderr, "P = %d, N = %d\n", p ,n);
	execute(p, n);
	n=2;
	n = (int)pow(2.0, n);
	fprintf(stderr, "P = %d, N = %d\n", p ,n);
	execute(p, n);
	n=3;
	n = (int)pow(2.0, n);
	fprintf(stderr, "P = %d, N = %d\n", p ,n);
	execute(p, n);
	n=4;
	n = (int)pow(2.0, n);
	fprintf(stderr, "P = %d, N = %d\n", p ,n);
	execute(p, n);
	fprintf(stderr, "Main program done, printing final minium values\n");
	//for(i=0; i<n; i++){
		//fprintf(stderr,"%d, ", minArray[i]);
	//}
	fprintf(stderr,"\n Program exiting\n");
	
	

}