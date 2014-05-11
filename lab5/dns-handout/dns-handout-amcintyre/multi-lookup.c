/******************************************************************************
 * FILE: multi-lookup.c
 * DESCRIPTION:
 *   A threaded DNS resolver.  Demonstrates thread creation and
 *   termination. and using provided util.h to resolve domain names to ip addresses.
 * AUTHOR: Aaron McIntyre
 * 
 ******************************************************************************/
	
#include "multi-lookup.h"

 int main(int argc, char* argv[]){
 	request_done = false;
 	int CPUNUM = sysconf(_SC_NPROCESSORS_ONLN);
 	FILE* outputfp = NULL;
    pthread_t requester_threads[argc-2];
  	pthread_t resolver_threads[CPUNUM*2];
    int rc;
    int i;

    /* Check Arguments */
    if(argc < MINARGS || argc > MAXARGS){
	fprintf(stderr, "Invalid Number of arguments(1-10): %d\n", (argc - 1));
	fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
	return EXIT_FAILURE;
    }

    /* Open Output File */
    outputfp = fopen(argv[(argc-1)], "w");
    if(!outputfp){
	perror("Error Opening Output File");
	return EXIT_FAILURE;
    }

    /* Initialize Queue */
    if(queue_init(&q, qSize) == QUEUE_FAILURE){
	fprintf(stderr,
		"error: queue_init failed!\n");
    }

    /*initialize mutex lock*/
    if (pthread_mutex_init(&qlock, NULL) != 0 || pthread_mutex_init(&flock, NULL)!= 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    // start requester threads
    
    
    /* Loop Through Input Files */
    for(i=1; i<(argc-1); i++){
    	//create 1 requester thread for each file name. max of 10 file names
    	rc = pthread_create(&(requester_threads[i-1]), NULL, Requester, argv[i]);
    	if (rc){
	    printf("ERROR; return code from pthread_create() is %d\n", rc);
	    exit(EXIT_FAILURE);
		}
    }

    //create resolver threads based on number of processors
    for(i=0; i < CPUNUM*2; ++i)
    {
    	rc = pthread_create(&(resolver_threads[i]), NULL, Resolver, outputfp);
    	if (rc){
	    printf("ERROR; return code from pthread_create() is %d\n", rc);
	    exit(EXIT_FAILURE);
		}
    }

    /* Wait for All requesterTheads to Finish */
    for(i=0;i<(argc-2);++i){
	pthread_join(requester_threads[i],NULL);
    }
    request_done = true;
    printf("All of the requester threads have completed!\n");

    for(i=0;i<CPUNUM*2;++i){
    pthread_join(resolver_threads[i],NULL);
    }
    printf("All of the resolver threads have completed!\n");

    //destory mutex locks
    fclose(outputfp);
    outputfp = NULL;
	pthread_mutex_destroy(&flock);
    pthread_mutex_destroy(&qlock);
    //clean up q
    queue_cleanup(&q);
    return 0;

 }

 void* Requester(void* filename)
{
	char* file = filename;
	FILE* inputfp = NULL;

	char hostname[SBUFSIZE];
    char errorstr[SBUFSIZE];
    char* payload;
    bool thishostinq;
    printf("filename is %s\n", file);
	/* Open Input File */
	inputfp = fopen(file, "r");
	if(!inputfp){
	    sprintf(errorstr, "Error Opening Input File: %s", file);
	    perror(errorstr);
	}	

	/* Read File and Process*/
	while(fscanf(inputfp, INPUTFS, hostname) > 0){
	    
	    
	    //run loop while hostname is not in the q.
	    while(!thishostinq){
	    	
	    	/*if q is full sleep random number between 0 and 100 microseconds. if q is not full copy 
	    	hostname to newly dynamically allocated char* payload. lock push payload unlock set 
	    	thishostinq to true to break out of while loop.*/
	    	pthread_mutex_lock(&qlock);
		    if(queue_is_full(&q)){
		    	pthread_mutex_unlock(&qlock);
		    	usleep(rand()%100);
		    }else{
		    	//hello
		    	payload = malloc(SBUFSIZE);
				strncpy(payload, hostname, SBUFSIZE);
			    printf("lock q\ninsert hostname:%s\n", hostname);
			    if(queue_push(&q, payload) == QUEUE_FAILURE){
				    fprintf(stderr,
					    "error: queue_push failed too full!\n"
					    "Payload %s\n", hostname);
				}
				pthread_mutex_unlock(&qlock);
				printf("unlock q\n");
				thishostinq = true;
			}	
		}
		thishostinq = false;
	}
	fclose(inputfp);
	inputfp = NULL;	
	printf("requester thread closing\n");
	return NULL;
}


void* Resolver(void* file)
{
	
	char firstipstr[INET6_ADDRSTRLEN];

	FILE* outputfp = file;

	
	printf("to start q is empty %d\nrequest is %d\n", queue_is_empty(&q), request_done);
	//while q is not empty and there are still requester threads
	while(!queue_is_empty(&q) || !request_done){

		
		printf("lock q for pop\n");
		pthread_mutex_lock(&qlock);
		printf("lock q done\n");
		char* hostname;
		if((hostname = queue_pop(&q)) == NULL){
			fprintf(stderr,
			"error: queue_pop failed!\n"
			"Payload: %s\n", hostname);
			pthread_mutex_unlock(&qlock);
			usleep(rand()%100);
		}else{
			pthread_mutex_unlock(&qlock);		
			printf("unlock pop q hostname: %s\n", hostname);
		  	// Lookup hostname and get IP string
		    if(dnslookup(hostname, firstipstr, sizeof(firstipstr)) == UTIL_FAILURE){
				fprintf(stderr, "dnslookup error: %s\n", hostname);
				strncpy(firstipstr, "", sizeof(firstipstr));
		    }
		    printf("unlock file\n");
		    pthread_mutex_lock(&flock);
		    /* Write to Output File */
		    printf("%s,%s\n", hostname, firstipstr);
		    fprintf(outputfp, "%s,%s\n", hostname, firstipstr);
		    pthread_mutex_unlock(&flock);
		    printf("unlock file\n");
		    printf("q is empty %d\nrequest is %d\n", !queue_is_empty(&q), !request_done);
		}
		free(hostname);
		hostname = NULL;
	}
	
	
	printf("resolver thread closing\n");
    return NULL;
}