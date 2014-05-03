/*
**	This example is based on the following tutorial: 
**	http://publib.boulder.ibm.com/infocenter/iseries/v5r4/index.jsp?topic=%2Fapis%2Fsigwait.htm
**
**	The tutorial code was modified to catch SIGUSR1 instead of SIGALRM, and the alarm was not set.  The
**	timstamp function was also modified to display the process ID as well as the string argument & timestamp.
**	Note that SIGUSR1 has the numeric value 10 on netlab2.
**
**	To execute, start the test program in one terminal window:  
**		./testsignal
**	and observe the <pid> of the process that is executing.
**
**	Send a kill segnal to the process from the command prompt in another terminal window: 
**		netlab2>kill <pid> 10
**
**	Next, write your own test program to send a SIGUSR signal to the <pid> value given on the command line.
**
**	MCM, 10/2011
*/

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>


void catcher( int sig ) {
    printf( "Signal catcher called for signal %d\n", sig );
    }

void timestamp( char *str ) {
	struct timeval mytv;
	
	gettimeofday(&mytv,NULL);

   printf( "The time in process %d %s is %s\n", getpid(), str, ctime(&(mytv.tv_sec)));
}

int main( int argc, char *argv[] ) {

    struct sigaction sigact;
    sigset_t waitset;
    int sig;
    int result = 0;

    sigemptyset( &sigact.sa_mask );
    sigact.sa_flags = 0;
    sigact.sa_handler = catcher;
    sigaction( SIGUSR1, &sigact, NULL );

    sigemptyset( &waitset );
    sigaddset( &waitset, SIGUSR1 );

    sigprocmask( SIG_BLOCK, &waitset, NULL );
    
    timestamp( "before sigwait()");

    result = sigwait( &waitset, &sig );
    if( result == 0 )
        printf( "sigwait() returned for signal %d\n", sig );
    else {
        printf( "sigwait() returned error number %d\n", errno );
        perror( "sigwait() function failed\n" );
    }

    timestamp( "after sigwait()" );

    return( result );
}
