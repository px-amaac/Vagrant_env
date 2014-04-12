/******************************************************************************
 * FILE: multi-lookup.h
 * DESCRIPTION:
 *   A threaded DNS resolver.  Demonstrates thread creation and
 *   termination. and using provided util.h to resolve domain names to ip addresses.
 * AUTHOR: Aaron McIntyre
 * 
 ******************************************************************************/

#ifndef _multi_lookup_h_
#define _multi_lookup_h_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "queue.h"

#define MINARGS 3
#define MAXARGS 10
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"
#define QSIZE 10
typedef enum { false, true } bool;

	queue q;
 	const int qSize = QSIZE;
 	pthread_mutex_t qlock;
 	pthread_mutex_t flock;
 	bool request_done;
 	void* Requester(void* filename);
 	void* Resolver(void* file);


#endif
