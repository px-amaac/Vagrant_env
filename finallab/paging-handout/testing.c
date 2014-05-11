#include <stdio.h> 
#include <stdlib.h>


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
                printf("page removed: %d\n", iter->page);
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
    printf("pop\n");
    if(tail == head){
        tail = head = NULL;
    }else if(tail != head){
        tail = tmp->prev;
        tail->next = NULL;
    }
    qsize--;
    return tmp;

}

void printq(struct lru_struct* head){
	struct lru_struct* iter = head;
	while (iter != NULL){
		printf("Page: %d, Proc %d\n", iter->page, iter->proc);
		iter = iter->next;
	}
}
int main(){
	int procs = 5;
	int i, j;
	head = tail = NULL;
	for (i=0; i<procs; i++){
		for(j=0; j<procs;j++){
			insert(j,i);
		}
	}
	insert(3,3);
	pop();
	insert(1,0);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	pop();
	printq(head);
	insert(1,0);
	printq(head);


	return 0;
}