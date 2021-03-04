#include <stdbool.h> 
#include "structs.h"

#ifndef QUEUE_H_   
#define QUEUE_H_

QUEUE *createQueue();
bool isEmpty(QUEUE* queue);
void enqueue(QUEUE* queue,  NODE *item); 
NODE* dequeue(QUEUE* queue); 
NODE* front(QUEUE* queue); 
void display(QUEUE* queue, char name[]);
QUEUE *sortQueueByArrivalTime(QUEUE* queue);
QUEUE *sortQueueByPriority(QUEUE* queue);
QUEUE *sortQueueByRemainingIOEndTime(QUEUE* queue);

int size(QUEUE* queue);

#endif
