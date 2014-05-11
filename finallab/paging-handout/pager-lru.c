/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

 struct lru_struct{
    struct lru_struct* next;
    struct lru_struct* prev; 
    int page;
    int proc;
};

static struct lru_struct* head;
static struct lru_struct* tail;
static int qsize;

/*findpage returns the page in page q if it is there otherwise it returns null*/
struct lru_struct* findpage(int* mPage, int* mProc){
    if(qsize != 0){
        struct lru_struct* iter = head;
        while(iter != NULL){
            //if you found the page you were trying to remove. 
            if(iter->proc == *mProc && iter->page == *mPage){

                return iter;
            }
            iter = iter->next;
        }
    }
    return NULL;
}

struct lru_struct* getlastpage(int mProc){
    if(qsize != 0){
        struct lru_struct* iter = head;
        while(iter != NULL){
            //if you found the page you were trying to remove. 
            if(iter->proc == mProc){
                return iter;
            }
            iter = iter->next;
        }
    }
    return NULL;
}

void insert(int mPage, int mProc){
    //if q is empty then add page
    if(head == NULL){
        head = tail = (struct lru_struct*) malloc(sizeof(struct lru_struct));
        head->next = NULL;
        head->page = mPage;
        head->proc = mProc;
        head->prev = NULL;
        qsize++;
        //printf("inserted to empty\n");
    //else q is not empty. look for page
    }else{

        struct lru_struct* tmp = findpage(&mPage, &mProc);
        if(NULL != tmp && head != tail && tmp != head){
            //we found the page were looking for and need to move it to front of q
            if(tmp == tail){
                tail = tmp->prev;
                tail->next = NULL;
            }
            else{
                tmp->next->prev = tmp->prev;
                tmp->prev->next = tmp->next;
            }
            //put tmp at front of q
                tmp->prev = NULL;
                tmp->next = head;
                head->prev = tmp;
                head = tmp;
        //else we did not find the page already in memory. create it and add it to front of q.
        }else if(NULL == tmp){
            tmp = head;
            head = (struct lru_struct*) malloc(sizeof(struct lru_struct));
            head->next = tmp;
            tmp->prev = head;
            head->page = mPage;
            head->proc = mProc;
            head->prev = NULL;
            qsize++;
            //printf("inserted to full\n");
        }
    }
    
}

void remove_page(struct lru_struct* iter){
    //if the q is not empty.
    if(head != NULL){
        /*//cycle through the q starting at the tail.
        while(iter != NULL){
            //if you found the page you were trying to remove. 
            if(iter->proc == mProc && iter->pc == mPc){*/
                //if that node is the only one then delete it and set head and tail pointers to null.
                if(iter->next == NULL && iter->prev == NULL){
                    tail = head = NULL;
                //else if that node is the tail
                }else if(iter->next == NULL){
                    tail = iter->prev;
                    tail->next = NULL;
                //else node is head
                }else if(iter->prev == NULL){
                    head = iter->next;
                    head->prev = NULL;
                //else the node is in the middle somewhere. 
                }else{
                    iter->prev->next = iter->next;
                    iter->next->prev = iter->prev;
                }
                //when finished removing break out of loop because we are done.
               // printf("page removed: %d\n", iter->page);
                free(iter);
                iter = NULL;
                qsize--;

                /*break;
            }
            //if you havent broken out of the loop then set iter to prev and move to next in q.
            iter = iter->prev;
        }   */
    }
}

struct lru_struct* pop(){
    struct lru_struct* tmp = tail;
    //printf("pop\n");
    if(tail == head){
        tail = head = NULL;
    }else if(tail != head){
        tail = tmp->prev;
        tail->next = NULL;
    }
    qsize--;
    return tmp;

}

void getPage(Pentry q[MAXPROCESSES], int proctmp, int pagetmp, int swappingout[MAXPROCESSES]){

    if(!q[proctmp].pages[pagetmp] /*&& swappingin[proctmp] == 0*/){
                //swapin failed so pop and swap in.               
        if(!pagein(proctmp, pagetmp)){
            if(!swappingout[proctmp]){
                struct lru_struct* lru_page = pop();
                if(lru_page != NULL){
                    //printf("pageout %d, proc: %d qsize: %d\n", lru_page->page, lru_page->proc, qsize);
                    if(!pageout(lru_page->proc, lru_page->page)) {
                        fprintf(stderr, "pageout(%d, %d) was not successful. qsize: %d\n", lru_page->proc, lru_page->page, qsize);      
                    }else{
                        swappingout[proctmp] = 1;
                    }
                    free(lru_page);
                }
            }                    
        }else{
            //swappingin[proctmp] = 1;
            swappingout[proctmp] = 0;
        }
        
    }

}

void pageit(Pentry q[MAXPROCESSES]) { 

    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int swappingout[MAXPROCESSES];
    static int swappingin[MAXPROCESSES];
    
    /* Local vars */
    int proctmp;
    int pagetmp;
    int pc;

    /* initialize static vars on first run 
    Im not sure if im using the timstamps so i am leaving them in for now.*/
    if(!initialized){
        head = tail = NULL;
        qsize = 0;
    	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
    	    swappingout[proctmp] = 0;
            swappingin[proctmp] = 0;
            /*for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
    		timestamps[proctmp][pagetmp] = 0; 
    	    }*/
    	}
	initialized = 1;
    printf("initialization finished qsize: %d\n", qsize);
    }

    /*if the page q is not empty then cycle through all pages starting from the tail. 
    if the pages processor is not active any more then remove the page from the q 
    and pageit out*/
    if(qsize != 0){
        //printf("qsize not 0: %d\n", qsize);
        struct lru_struct* iter = tail;
        while(iter != NULL){
            //printf("proc activity %ld, %d\n", q[iter->proc].active, iter->proc);
            if (!q[iter->proc].active){
                //printf("proc not active removing \n");
                struct lru_struct* tmp = iter->prev;
                pageout(iter->proc, iter->page);
                remove_page(iter);
                iter = tmp;
                continue;
            }
        iter = iter->prev;
        }
    }

    /*Go through all of the processors*/
    for(proctmp=0; proctmp<MAXPROCESSES; ++proctmp){

        /*Check if each processor is active.*/
        if(q[proctmp].active){

            //get page needed
            pc = q[proctmp].pc;
            pagetmp = pc/PAGESIZE;

            //printf("proc %d active, page %d\n", proctmp, pagetmp);
            //if page is allocated
            if(q[proctmp].pages[pagetmp]){
                insert(pagetmp, proctmp);
                struct lru_struct* lastpage = getlastpage(proctmp);
                int i, dif;
                if (lastpage != NULL){
                    if(pagetmp < lastpage->page){
                        printf("jumped backwards\n");
                        dif = lastpage->page - pagetmp;
                        if(dif > 20){
                            dif=20;
                            printf("wayback\n");
                        }
                        for(i = pagetmp+1; i<=dif; ++i){
                            getPage(q, proctmp, i, swappingout);
                        }
                    }
                }
                //printf("insert page%d, proc %d\n", pagetmp, proctmp);
                //if page allocated and swapping in flag is 1 then page has been swapped in.
                /*if(swappingin[proctmp]){
                    swappingin[proctmp] = 0;
                }*/
            } else {
                getPage(q, proctmp, pagetmp, swappingout);

            }
        }
    }
    /* advance time for next pageit iteration */
    //printf("tick: %d\n", tick);
    tick++;
} 
