#include <stdbool.h> 
#include "processes.h"

#ifndef QUEUE_H_   
#define QUEUE_H_

typedef struct Node {
	struct Node *prev;
	PCB pcb;
} NODE;


typedef struct Queue { 
    NODE *head;
	NODE *tail;
} Queue; 

Queue *createQueue();
bool isEmpty(Queue* queue);
void enqueue(Queue* queue,  NODE *item); 
NODE* dequeue(Queue* queue); 
NODE* front(Queue* queue); 
void display(Queue* queue, char name[]);
Queue *sortQueueByArrivalTime(Queue* queue);
Queue *sortQueueByPriority(Queue* queue);
Queue *sortQueueByRemainingIOEndTime(Queue* queue);

int size(Queue* queue);

#endif
