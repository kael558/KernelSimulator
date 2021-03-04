#ifndef PROCESSES_H_   
#define PROCESSES_H_


typedef struct Processes {
	int PID;
	
	int arrival_time;
	int remaining_CPU_time; //counts down in runningProcess
	
	int IO_start_time; //IO frequency
	int remaining_IO_start_time; //counts down in running process
	
	int IO_end_time; //constant
	int remaining_IO_end_time; //counts down in waiting queue
	
	int wait_time; 
	int burst_time;

	int priority;
} PCB;

typedef struct Node {
	struct Node *prev;
	PCB pcb;
} NODE;


typedef struct Queue { 
    NODE *head;
	NODE *tail;
} QUEUE; 

typedef struct Partition {
	int memory;
	NODE *runningProcess;
} PARTITION;

#endif
