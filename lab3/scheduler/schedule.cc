#include "schedule.h"

struct node {
	int id; //id for the node. 
	struct node *next; //pointer to the next node.
};

struct node *root = NULL;
struct node *current = NULL;

/*
 * Functon to add a process to the scheduler
 * @Param tid - the ID for the process/thread to be added to the 
 *      scheduler queue
 * @return true/false response for if the addition was successful
 */
int addProcess(int tid){
	struct node *iter = NULL;
	iter = root;
	if(root == NULL){ //if root is NULL then there is nothing in the list
		root = (struct node *) malloc( sizeof(struct node)); //create the first node 
		current = root;// set iter to root.
		iter = root;
	}else{
		while(true)
		{
			if(iter->next != NULL) //if iter next is not NULL then we are not at the end of the list.
			{
				iter = iter->next; //set iter to iters next to move through the list.
			}
			else{ //iters next node is NULL so we are at the end of the list and need to add our process.
				iter->next = (struct node *) malloc( sizeof(struct node)); //create a new process node and link it to iters next
				iter = iter->next;
				break; //break out of the loop we have added the node
			}
		}
	}
	iter->id = tid;
	
	iter->next = NULL;
	
	return 0;
}
/*
 * Function to remove a process from the scheduler queue
 * @Param tid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcess(int tid){
	struct node *iter = NULL;
	iter = root;
	if(root == NULL)
		return false;
	else if(root->id == tid) //if the root node is our target we need to delete it and reset the root node.
	{
		if (root == current){
			current = current->next;
		}
		iter = root->next; //set iter to the new root.
		free(root); //delete the root node.
		root=iter; //set root to the new root node.
	}
	else{ //the target node is in the list or not but it is not root.
		while (true){
			if(iter->next == NULL) {//if we peek at the next node and it is not there then we have
				//reached the end of the list and didnt find our target. return false.
				return false;
			}
			else if(iter->next->id == tid) //peek at the next node. if next node is our target
			{
				struct node *tmp = iter->next->next; //set a temp node to the one following our target.
				free(iter->next); //free the node in between
				iter->next = tmp; //set iters next to the tmp to relink the list.
				return true; //we found and deleted the target node return true.
			}else{
				iter = iter->next; //the next node is not NULL and we have not found our target yet.
			}
		}
	}
	return 0;
}
/*
 * Function to get the next process from the scheduler
 * @Return returns the thread id of the next process that should be 
 *      executed, returns -1 if there are no processes
 */
int nextProcess(){
	if (root == NULL)
	{
		return -1;
	}
	else
	{
		int result = current->id;
		if(current->next != NULL)
			current = current->next;
		else
			current = root;
		return result;
	}
}

