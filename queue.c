#include <limits.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 

#include "queue.h"


QUEUE *createQueue() 
{ 
    QUEUE* queue = (QUEUE*)malloc(sizeof(QUEUE)); 
	queue->head = NULL;
	queue->tail = NULL;
    return queue; 
} 
  
bool isEmpty(QUEUE* queue) 
{ 
    return (queue->head == NULL); 
} 
  
// Function to add an item to the queue. 
// It changes rear and size 
void enqueue(QUEUE* queue, NODE *item) 
{ 
	if (queue == NULL || item == NULL)
		return;

	item->prev = NULL;
	if (isEmpty(queue)){
		queue->head = item;
	} else {
		queue->tail->prev = item;
	}
	queue->tail = item;
	return;
} 
  
// Function to remove an item from queue. 
// It changes front and size 
NODE* dequeue(QUEUE* queue) 
{ 
	if (isEmpty(queue))
		return NULL;

	NODE *item = queue->head;
	queue->head = item->prev;
	return item;
} 
  

NODE* front(QUEUE* queue)
{
	return queue->head;
}


void display(QUEUE* queue, char name[])
{
	printf("%s Queue: \n", name);
	for (NODE* iter = queue->head; iter!=NULL; iter = iter->prev)
	{ 
		printf("%i %i %i %i %i %i %i %i\n", 
			iter->pcb.PID, 
			iter->pcb.arrival_time, 
			iter->pcb.remaining_CPU_time, 
			iter->pcb.IO_start_time, 
			iter->pcb.remaining_IO_start_time, 
			iter->pcb.IO_end_time,
			iter->pcb.remaining_IO_end_time, 
			iter->pcb.priority);
	}
	return;
}

void displayNode(NODE* iter)
{
	printf("%i %i %i %i %i %i %i %i\n", 
			iter->pcb.PID, 
			iter->pcb.arrival_time, 
			iter->pcb.remaining_CPU_time, 
			iter->pcb.IO_start_time, 
			iter->pcb.remaining_IO_start_time, 
			iter->pcb.IO_end_time,
			iter->pcb.remaining_IO_end_time, 
			iter->pcb.priority);
}

QUEUE *sortQueueByArrivalTime(QUEUE* queue)
{
	for (NODE* iter1 = queue->head; iter1!=NULL; iter1 = iter1->prev){ 
		for (NODE* iter2 = queue->head; iter2!=queue->tail; iter2 = iter2->prev){
			if (iter2->pcb.arrival_time > iter2->prev->pcb.arrival_time){
				PCB temp = iter2->pcb;
				iter2->pcb = iter2->prev->pcb;
				iter2->prev->pcb = temp;
			}
		}
	}
	return queue;
}


QUEUE *sortQueueByPriority(QUEUE* queue)
{
	for (NODE* iter1 = queue->head; iter1!=NULL; iter1 = iter1->prev){ 
		for (NODE* iter2 = queue->head; iter2!=queue->tail; iter2 = iter2->prev){
			if (iter2->pcb.priority > iter2->prev->pcb.priority){
				PCB temp = iter2->pcb;
				iter2->pcb = iter2->prev->pcb;
				iter2->prev->pcb = temp;
			}
		}
	}
	return queue;
}

QUEUE *sortQueueByRemainingIOEndTime(QUEUE* queue)
{
	for (NODE* iter1 = queue->head; iter1!=NULL; iter1 = iter1->prev){ 
		for (NODE* iter2 = queue->head; iter2!=queue->tail; iter2 = iter2->prev){
			if (iter2->pcb.remaining_IO_end_time > iter2->prev->pcb.remaining_IO_end_time){
				PCB temp = iter2->pcb;
				iter2->pcb = iter2->prev->pcb;
				iter2->prev->pcb = temp;
			}
		}
	}
	return queue;
}

int size(QUEUE* queue)
{
	int size = 0;
	for (NODE* iter = queue->head; iter!=NULL; iter = iter->prev, size++);
	return size;
}

