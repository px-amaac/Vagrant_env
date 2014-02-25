// 
// 
// 
// Aaron McIntyre 001153075
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>
#include <limits.h>
#include <sys/time.h>
#include <float.h>

#include "helper-routines.h"

/*Define Global Variables*/
pid_t   childpid;
timeval t1, t2;
int numtests;
double elapsedTime;

//sigusr1 for sending from child to parent
void sigusr1_handler(int sig) 
{	
	
}
//sigusr2 for sending from parent to child
void sigusr2_handler(int sig) 
{ 
	
}

void sigint_handler(int sig){
	
}
double toDouble(struct timeval *timevalue)
{
	double result;
	result = timevalue->tv_sec * 1000.0;      // sec to ms
	result += timevalue->tv_usec / 1000.0;   // us to ms
	return result;
}


void print(double min, double max, double sum, double numtest)
{
	pid_t pid = getpid();
	double average = sum/numtests;
	if (childpid != 0)
	printf("Parent's Results for Pipe IPC mechanisms\n");
	else
	printf("Child's Results for Pipe IPC mechanisms\n");
	printf("Process ID is %d, Group ID is %d\n", pid, getpgid(pid));
	printf("Round trip times\n");
	printf("Average %f\n", average);
	printf("Maximum %f\n", max);
	printf("Minimum %f\n", min);
}
//
// main - The main routine 
//
int main(int argc, char **argv){
	//Initialize Constants here
	
    //variables for Pipe
	int fd1[2],fd2[2], nbytes;
	char readbuffer[1];	
	//byte size messages to be passed through pipes	
	char    childmsg[] = "1";
	char    parentmsg[] = "2";
	char    quitmsg[] = "q";
    
    /*Three Signal Handlers You Might Need
     *
     *I'd recommend using one signal to signal parent from child
     *and a different SIGUSR to signal child from parent
     */
    Signal(SIGUSR1,  sigusr1_handler); //User Defined Signal 1
    Signal(SIGUSR2,  sigusr2_handler); //User Defined Signal 2
    Signal(SIGINT, sigint_handler);
    
    //Default Value of Num Tests
    numtests=10000;
    //Determine the number of messages was passed in from command line arguments
    //Replace default numtests w/ the commandline argument if applicable 
    if(argc<2){
		//printf("Not enough arguments\n");
		exit(0);
	}else if(argc == 3)
	numtests = atoi(argv[2]);
    
    //printf("Number of Tests %d\n", numtests);
    // start timer
    pipe(fd1); 		//create pipe 1 for sending from child to parent
    pipe(fd2); 		//create pipe 2 for sending from parent to child
    timeval rt1;
    timerclear(&rt1);
    timeval rt2;
    timeval rtdiff;
    timeval sum;
    double min = DBL_MAX;
	double max = 0.0;
    childpid = fork(); 		//fork the process getting the childpid. 
    						//child pid is -1 if error, 0 if child and the child pid if its the parent.

	//start timer
	gettimeofday(&t1, NULL);
	//if running benchmark on pipes.  
	if(strcmp(argv[1],"-p")==0)
	{
		//fork check if child. If child read from parent start timer, write parent
		if(childpid == -1) //error on fork
		{
			perror("fork");
			exit(1);
		}

		if(childpid == 0) //child process
		{
			close(fd1[0]); //close input side of pipe1. sending from child to parent
			close(fd2[1]); //close output side of pipe2. recieve from parent on the input side of pipe.
			
			while(true)
			{
				read(fd2[0], readbuffer, sizeof(readbuffer)); //read will block until it had data
				//
				if(strcmp(readbuffer, quitmsg) == 0)
				{
					printf("read quitMSG: %s\n", readbuffer);
					print(min, max, toDouble(&sum), double(numtests));
					break;
				}
				else if(strcmp(readbuffer, parentmsg) == 0)
				{

					//checks if start timer is null
					//if the child round trip timer is not null then stop the RT timer and 
					//calculate the round trip time, see if its the min, see if its the max.
					if (rt1.tv_usec > 0){
						gettimeofday(&rt2, NULL);
						timersub(&rt2, &rt1, &rtdiff); //get the time elapsed 
						timeradd(&sum, &rtdiff, &sum); //add the difference to the running total. 
						//This will be different from the elapsed time because the elapsed time takes 
						//all the calculations of every iteration into account
						//compare min and max
						double rt = toDouble(&rtdiff);

						//compare current rt time with the min and max set the min or max if the conditions are met.
						if(rt < min)
							min = rt;
						if(rt > max)
							max = rt;
						printf("child: max %f, min %f, rt %f\n", max, min, rt);
					}	
				}
				//parent sent a message
				//start new timer and write to parent.
				gettimeofday(&rt1, NULL);
				printf("Send Child Msg to parent\n");
				write(fd1[1], childmsg, (strlen(childmsg)+1));	
			}
			
			exit(0);
		}
		else //parent
		{

			int counter = numtests;
			close(fd1[1]); //close output side of pipe1. recieving from child
			close(fd2[0]); //close input side of pipe2. sending from parent.
			while (counter >= 0) //cycle through the tests
			{
				if(rt1.tv_usec > 0)
				{
					read(fd1[0], readbuffer, sizeof(readbuffer));
					printf("Read in Parent: %s\n", readbuffer);
					if(strcmp(readbuffer, childmsg) == 0)
					{
						gettimeofday(&rt2, NULL); //stop
						timersub(&rt2, &rt1, &rtdiff); //get the time elapsed 
						
						timeradd(&sum, &rtdiff, &sum); //add the difference to the running total. 
						//This will be different from the elapsed time because the elapsed time takes 
						//all the calculations of every iteration into account
						//compare min and max
						double rt = toDouble(&rtdiff);

						//compare current rt time with the min and max set the min or max if the conditions are met.
						//printf("max is %f\n", max);
						if(rt < min)
							min = rt;
						if(rt > max)
							max = rt;
						printf("parent: max %f, min %f, rt %f\n", max, min, rt);
					}
				}
				if(counter != 0){
					printf("Send  parent msg to child\n");
					gettimeofday(&rt1, NULL);
					write(fd2[1], parentmsg, (strlen(parentmsg)+1));	
				}else{
					int status;
					printf("Send Quit Msg\n");
					write(fd2[1], quitmsg, (strlen(quitmsg)+1));
					waitpid(childpid, &status, 0);
				}
				counter--;
				//printf("Counter %d\n", counter);
			}
			
			print(min, max, toDouble(&sum), double(numtests));
		}
		
		// stop timer
		gettimeofday(&t2, NULL);

		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		printf("Elapsed Time %f\n", elapsedTime);
		exit(0);
	}
	if(strcmp(argv[1],"-s")==0){
		//code for benchmarking signals over numtests
		
		
		// stop timer
		gettimeofday(&t2, NULL);

		// compute and print the elapsed time in millisec
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		printf("Elapsed Time %f\n", elapsedTime);
	}
	
}
  










