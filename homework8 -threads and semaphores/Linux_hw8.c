#include <pthread.h>	//thread subroutines
#include <stdio.h>	//standard I/O subroutines
#include <stdlib.h>	//standard library subroutines
#include <time.h>	//time subroutines, necessary for nanosleep()
#include <math.h>	//necessary for power function pow()

#define BILLION  1000000000L;

//global variables
struct timespec start, stop;
int n, p, retval;
static int *randArray;
static int *minArray;
static int *sortedArray;
static int *finalArray;
double seqTime, conTime;

struct threadStruct{	//struct for thread variables
	pthread_t threadID;
	int threadNum;
	int localIndex;
	int count;
};

//sort the array in the range (start-end) using insertion sort
void insertionSort(int *array, int start, int end){
	int i,j,temp;
	for(i=start;i<end;i++){
		for(j = i;j<end;j++){
			if(array[i] > array[j]){
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;				
			}
		}
	}	
}

//get the next overall smallest number from the sorted partitions
int getNext(struct threadStruct *structOfThreads){
	int i, smallest, minVal;
	minVal = 0;
	for(i=1;i<n;i++){			
		if(sortedArray[minArray[i]] < sortedArray[minArray[minVal]]){
			minVal = i;
		}
	}
	
	smallest = sortedArray[minArray[minVal]];
	if(structOfThreads[minVal].count > 1){
		minArray[minVal]++;
		structOfThreads[minVal].count--;
	}else{
		sortedArray[minArray[minVal]] = RAND_MAX;
	}	
	return(smallest);
}

void execute(void *arg) {
	int x, start, end; 
	struct threadStruct *temp = arg;	
	x = p/n;
	temp->count = x;
	start = temp->threadNum * x;
	end = start + x;	
	insertionSort(sortedArray,start,end);
	minArray[temp->threadNum] = start;
}

void testDriver(struct threadStruct *structOfThreads){
	int i,isSorted;
	isSorted = 1;	
	/*
	 *Sequential Execution
	 */
	memcpy(sortedArray,randArray,sizeof(int)*p);
	// start sequential execution time
	if(clock_gettime(CLOCK_REALTIME, &start) == -1) { 
      perror("ERROR: clock gettime");
      exit(-1);
    } 
	insertionSort(sortedArray,0,p);
	// end sequential execution time
	if(clock_gettime(CLOCK_REALTIME, &stop) == -1) {
      perror("ERROR: clock gettime");
      exit(-1);
    }		
	
	// test to make sure the array is really sorted
	for(i=0;i<p-1;i++){
		if(sortedArray[i] > sortedArray[i+1]){
			isSorted = 0;
			break;
		}
	}	
	if(isSorted){
		fprintf(stderr,"Sequential Execution sorted the array CORRECTLY\n");
	}else{
		fprintf(stderr,"***ERROR: Sequential Execution FAILED to sort the array!!!\n");
	}
	//print execution time
	seqTime = (stop.tv_sec - start.tv_sec)
             + (double)(stop.tv_nsec - start.tv_nsec) / (double)BILLION;
	fprintf(stderr,"Sequential Execution Time: %.9f seconds\n\n",seqTime);
	
	/*
	 *Concurrent Executions (for 1 to 16 threads)
	 */	
	for(n=1;n<=16;n*=2){   //loop of concurrent execution for (2**0 to 2**4) threads
		isSorted = 1;
		memcpy(sortedArray,randArray,sizeof(int)*p);
		
		//start concurrent execution time	
		if(clock_gettime(CLOCK_REALTIME, &start) == -1) { 
		  perror("ERROR: clock gettime");
		  exit(-1);
		}
		
		for(i=0;i<n;i++){	//thread creation
			structOfThreads[i].threadNum = i;
			if ((retval = pthread_create(&structOfThreads[i].threadID,NULL, (void *)execute, &structOfThreads[i])) < 0){
				perror("Can't create producer thread\n");
				exit(-1);
			}//return 0 if creation successful		
		}
			
		for(i=0; i<n; i++){	//loop to close threads
			if(pthread_join(structOfThreads[i].threadID,NULL)){
				perror("Error terminating producer thread\n");
				exit(-1);
			}
		}
				
		//do the final mergesort on the sorted partitions
		for(i=0;i<p;i++){		
			finalArray[i] = getNext(structOfThreads);
		}	
		
		//end concurrent execution time
		if(clock_gettime(CLOCK_REALTIME, &stop) == -1) {
		  perror("ERROR: clock gettime");
		  exit(-1);
		}
		
		//print the number of threads
		fprintf(stderr,"2^P = %d, 2^N = %d\n",p,n);			
		
		//test to make sure the array is really sorted
		for(i=0;i<p-1;i++){
			if(finalArray[i] > finalArray[i+1]){
				isSorted = 0;
				break;
			}
		}	
		if(isSorted){
			fprintf(stderr,"Concurrent Execution sorted the array CORRECTLY\n");
		}else{
			fprintf(stderr,"***ERROR: Concurrent Execution FAILED to sort the array!!!\n");
		}
		//print execution time
		conTime = (stop.tv_sec - start.tv_sec)
				 + (double)(stop.tv_nsec - start.tv_nsec) / (double)BILLION;
		fprintf(stderr,"Concurrent Execution Time with %d threads: %.9f seconds\n\n",n,conTime);
	}
}

main(int argc, char *argv[]) {	
	struct threadStruct *structOfThreads;
	int i;
	
	if(argc != 2){	//check for correct number of input arguments
		perror("Error: Program takes exactly 2 input arguments!!!");
		exit(-1);
	}	
	p=atoi(argv[1]);	//command line input
	p = pow(2,p);
	
	//memory allocation
	structOfThreads = calloc(16,sizeof(struct threadStruct));
	randArray = calloc(p,sizeof(int));
	sortedArray = calloc(p,sizeof(int));
	finalArray = calloc(p,sizeof(int));
	minArray = calloc(16,sizeof(int));
	
	for(i=0;i<p;i++){	//create random array
		randArray[i] = rand()%RAND_MAX;  //RAND_MAX in POSIX is 2^31, large enough to avoid duplicates
	}	
	
	fprintf(stderr,"\nMain Program beginning!\n\n");	
	testDriver(structOfThreads);	//run program
	fprintf(stderr,"Main Program finishing!\n");
}