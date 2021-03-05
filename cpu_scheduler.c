#include "queue.h"

#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>  
#include <string.h>

/*
Creates the nodes consisting of a process which gets put into the initial queue
*/
NODE* makeNode(int PID, int arrival_time, int r_CPU_time, int IO_freq, int IO_duration, int priority, int memory)
{
	NODE *p = (NODE*) malloc(sizeof (NODE));
	p->pcb.PID = PID;
	p->pcb.arrival_time = arrival_time;
	p->pcb.remaining_CPU_time = r_CPU_time;
	p->pcb.IO_start_time = IO_freq; 
	p->pcb.remaining_IO_start_time = IO_freq;
	p->pcb.IO_end_time = IO_duration; 
	p->pcb.remaining_IO_end_time = IO_duration;
	p->pcb.burst_time = r_CPU_time;
	p->pcb.wait_time = 0;
	p->pcb.priority = priority;
	p->pcb.memory = memory;
	return p;
}


/*
Writes the transitions
*/
void writeTransition(int tick, char oldState[], char newState[], NODE* p, FILE* file_ptr){
	printf("%i %i %s %s\n", tick, p->pcb.PID, oldState, newState); 
	fprintf(file_ptr,"%i %i %s %s\n", tick, p->pcb.PID, oldState, newState);
}


/*
Writes the memory transitions
*/
void recordMem(PARTITION *partitions, int num_partitions, int* avg_mem_used, int* avg_free_mem, int* avg_usable_mem, int* n, FILE* file_ptr){
	int total_memory_used = 0;
	int total_memory_free = 0;
	int total_memory_usable = 0;
	
	
	for (int i = 0; i < num_partitions; i++){
		if (partitions[i].runningProcess != NULL){
			total_memory_used+= partitions[i].runningProcess->pcb.memory;
			total_memory_free+=(partitions[i].memory - partitions[i].runningProcess->pcb.memory);
		} else {
			total_memory_free+=partitions[i].memory;
			total_memory_usable+=partitions[i].memory;
		}
	}


	(*n)+=1;
	int p = *n;
	int x1 = *avg_mem_used;
	int x2 = *avg_free_mem;
	int x3 = *avg_usable_mem;
	*avg_mem_used = ((x1 * (p - 1)) + total_memory_used)/ (p);
	*avg_free_mem = ((x2 * (p - 1)) + total_memory_free)/ (p);
	*avg_usable_mem = ((x3 * (p - 1)) + total_memory_usable)/ (p);
}


/*
Creates the inputProcessList which holds the processes until they reach the arrival time
*/
QUEUE* createInputProcessList(char* filename, bool hasIO, bool hasPriority, bool hasMemory){
	QUEUE* inputProcessList = createQueue();
	FILE *fp;
	char str[100];

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Could not open file %s",filename);
        exit(EXIT_FAILURE);
	}
	
	int pInfo[7] = {-1};

	while (fgets(str, 100, fp) != NULL){
		int i = 0;
		
		for (char* token = strtok(str, " "); token!=NULL; token=strtok(NULL, " ")){
			pInfo[i++] = atoi(token);
		}


		if  (!hasIO && !hasPriority && hasMemory){
			pInfo[6] = pInfo[3];
			pInfo[3] = -1;
		} else if  (!hasIO && hasPriority && !hasMemory){
			pInfo[5] = pInfo[3];
			pInfo[3] = -1;
		} else if  (!hasIO && hasPriority && hasMemory){
			pInfo[5] = pInfo[3];
			pInfo[6] = pInfo[4];
			pInfo[3] = -1;
			pInfo[4] = -1;
		} else if  (hasIO && !hasPriority && hasMemory){
			pInfo[6] = pInfo[5];
			pInfo[5] = -1;
		} 

		NODE* p = makeNode(pInfo[0], pInfo[1], pInfo[2], pInfo[3], pInfo[4], pInfo[5], pInfo[6]);

		enqueue(inputProcessList, p);
		
	}
	fclose(fp);

	return inputProcessList;
}


/*
Checks if a process from the inputProcessList is ready to be placed into the queue
*/
void handleArrivalOfProcess(QUEUE* inputProcessList, QUEUE* readyQueue, int tick, FILE* outFilePtr){
	while (!isEmpty(inputProcessList) && inputProcessList->head->pcb.arrival_time == tick){
		enqueue(readyQueue, dequeue(inputProcessList));
	}
}

/*
Simulates a CPU tick which runs a singular process called runningProcess
*/
void runMainCPUTick(PARTITION* partitions, int num_partitions){
	for (int i = 0; i < num_partitions; i++)
		if (partitions[i].runningProcess != NULL){
			partitions[i].runningProcess->pcb.remaining_CPU_time--;
			partitions[i].runningProcess->pcb.remaining_IO_start_time--;
		}
}

/*
Simulates the IO CPUs which process everything in the waitingQueue
*/
void runIOCPUTick(QUEUE* waitingQueue){
	for (NODE* iter = waitingQueue->head; iter!=NULL; iter = iter->prev){ //IO CPUs
		iter->pcb.remaining_IO_end_time--;
		iter->pcb.wait_time++;
	}

}

/*
Calls the respective CPU tick functions
*/
void handleTick(PARTITION* partitions, int num_partitions, QUEUE* waitingQueue){
	runMainCPUTick(partitions, num_partitions);
	runIOCPUTick(waitingQueue);
}

/*
calculates gcd between two numbers using euclids algo
*/
int gcd(int n1, int n2){
	while(n1!=n2){
        if(n1 > n2)
            n1 -= n2;
        else
            n2 -= n1;
    }
	return n1;
}





/*
analyzes the processes in the terminated queue, printing the metrics
*/
void analysis(QUEUE* terminatedQueue, int totalTime, FILE* file_ptr){
	int num_processes = size(terminatedQueue);

	double turnaround = 0.0;

	double wait = 0.0;

	for (NODE* iter = terminatedQueue->head; iter!=NULL; iter = iter->prev){ 
		wait+=iter->pcb.wait_time;
		turnaround+= (iter->pcb.wait_time + iter->pcb.burst_time);
		printf("id: %d -- wait time: %d  --  burst_time: %d\n", iter->pcb.PID, iter->pcb.wait_time, iter->pcb.burst_time);
		fprintf(file_ptr,"id: %d -- wait time: %d  --  burst_time: %d\n", iter->pcb.PID, iter->pcb.wait_time, iter->pcb.burst_time);
	}
	int div = gcd(num_processes, totalTime);

	printf("Throughput: %d processes per %dms", num_processes, totalTime);
	fprintf(file_ptr,"Throughput: %d processes per %dms", num_processes, totalTime);
	if (div != 1){
		printf(" -> %d processes per %dms", num_processes/div, totalTime/div);
		fprintf(file_ptr," -> %d processes per %dms", num_processes/div, totalTime/div);
	}
	printf("\nAvg Turnaround %f\nAvg Wait %f\n",  turnaround/num_processes, wait/num_processes);
	fprintf(file_ptr,"\nAvg Turnaround %f\nAvg Wait %f\n",  turnaround/num_processes, wait/num_processes);
}


void analysisMem(int* avg_mem_used, int* avg_free_mem, int* avg_usable_mem, FILE* file_ptr){
	fprintf(file_ptr,"\nAvg Mem_Used %d\nAvg Free Mem %d\nAvg Mem Usable %d\n", *avg_mem_used, *avg_free_mem, *avg_usable_mem);
}


bool areRunning(PARTITION *partitions, int num_partitions){
	for (int i = 0; i < num_partitions; i++){
		if (partitions[i].runningProcess != NULL){
			return true;
		}
	}
	return false;
}


int main(int argc, char *argv[]){ 
	char inputFile[100], outputFile[100];

	bool isRoundRobin = false, isPriority = false, isMem = false; //for simulation
	bool hasIO = false, hasPriority = false, hasMemory = false; //for input
	int roundRobinTicks = 3;
	int remainingRoundRobinTicks = roundRobinTicks;

	PARTITION* partitions = NULL;
	int num_partitions = 1;

	int avg_mem_used = 0, avg_free_mem = 0, avg_usable_mem = 0, n = 0;

	if (argc>1){
		strcpy(inputFile, argv[1]);

		if (strcmp(argv[2], "true") == 0)
			hasIO = true;
		if (strcmp(argv[3], "true") == 0)
			hasPriority = true;
		if (strcmp(argv[4], "true") == 0)
			hasMemory = true;
		
		
		strcpy(outputFile, argv[5]);
	
		if (strcmp(argv[6], "mem1") == 0){
			num_partitions = 1;
			PARTITION ps[num_partitions];
			ps[0].memory = 999999;
			ps[0].id = 1;
			ps[0].runningProcess = NULL;
			partitions = ps;
			
		} else if (strcmp(argv[6], "mem2") == 0){
			num_partitions = 4;
			PARTITION ps[num_partitions];
			ps[0].memory = 500;
			ps[1].memory = 250;
			ps[2].memory = 150;
			ps[3].memory = 100;

			ps[0].id = 1;
			ps[1].id = 2;
			ps[2].id = 3;
			ps[3].id = 4;	

			ps[0].runningProcess = NULL;
			ps[1].runningProcess = NULL;
			ps[2].runningProcess = NULL;
			ps[3].runningProcess = NULL;	
			partitions = ps;
			isMem = true;

		} else if (strcmp(argv[6], "mem3") == 0){
			num_partitions = 4;
			PARTITION ps[num_partitions];
			ps[0].memory = 300;
			ps[1].memory = 300;
			ps[2].memory = 350;
			ps[3].memory = 50;

			ps[0].id = 1;
			ps[1].id = 2;
			ps[2].id = 3;
			ps[3].id = 4;	

			ps[0].runningProcess = NULL;
			ps[1].runningProcess = NULL;
			ps[2].runningProcess = NULL;
			ps[3].runningProcess = NULL;		
			partitions = ps;
			isMem = true;
;
		}

		if (strcmp(argv[7], "roundRobin") == 0){
			isRoundRobin = true;
		} else if (strcmp(argv[7], "priority") == 0){
			isPriority = true;
		}


	} else { //for local execution

		strcpy(inputFile, "inputs3/input_CPU_High_Mem.txt");
		strcpy(outputFile, "test.txt");
		num_partitions = 4;
			PARTITION ps[num_partitions];
			ps[0].memory = 300;
			ps[1].memory = 300;
			ps[2].memory = 350;
			ps[3].memory = 50;

			ps[0].id = 1;
			ps[1].id = 2;
			ps[2].id = 3;
			ps[3].id = 4;	

			ps[0].runningProcess = NULL;
			ps[1].runningProcess = NULL;
			ps[2].runningProcess = NULL;
			ps[3].runningProcess = NULL;		
			partitions = ps;
			isMem = true;
		hasMemory = true;
		//isRoundRobin = true;
		//isPriority = true;*/
	}

	
	FILE* outFilePtr = fopen(outputFile, "w");

	QUEUE* inputProcessList = createInputProcessList(inputFile, hasIO, hasPriority, hasMemory);
    QUEUE* readyQueue = createQueue(); 
	QUEUE* waitingQueue = createQueue(); 
	QUEUE* terminatedQueue = createQueue(); //for use in analysis

	sortQueueByArrivalTime(inputProcessList);

	int tick = 0;

	for (; !isEmpty(readyQueue) || !isEmpty(waitingQueue) || areRunning(partitions, num_partitions) || !isEmpty(inputProcessList); tick++){
		
		handleArrivalOfProcess(inputProcessList, readyQueue, tick, outFilePtr);
		
		handleTick(partitions, num_partitions, waitingQueue);
	
	
		//CPU scheduler - see if process in waiting queue is finished processing
		sortQueueByRemainingIOEndTime(waitingQueue);
		while (!isEmpty(waitingQueue) && waitingQueue->head->pcb.remaining_IO_end_time == 0){
			waitingQueue->head->pcb.remaining_IO_start_time = waitingQueue->head->pcb.IO_start_time;
			writeTransition(tick, "WAITING", "READY", waitingQueue->head, outFilePtr);
			enqueue(readyQueue, dequeue(waitingQueue));
		}

		//see if running processes is terminated or needs IO CPU
		for (int i = 0; i < num_partitions; i++){
			if (partitions[i].runningProcess != NULL && partitions[i].runningProcess->pcb.remaining_CPU_time == 0){
				writeTransition(tick, "RUNNING", "TERMINATED", partitions[i].runningProcess, outFilePtr);
				enqueue(terminatedQueue, partitions[i].runningProcess);
				partitions[i].runningProcess = NULL;
			} else if (partitions[i].runningProcess != NULL && partitions[i].runningProcess->pcb.remaining_IO_start_time == 0){
				writeTransition(tick, "RUNNING", "WAITING", partitions[i].runningProcess, outFilePtr);
				partitions[i].runningProcess->pcb.remaining_IO_end_time = partitions[i].runningProcess->pcb.IO_end_time; 
				enqueue(waitingQueue, partitions[i].runningProcess);
				partitions[i].runningProcess = NULL;
			}
		}


		//if is priority is enabled, check if process in ready queue has higher priorirty
		if (isPriority){
			if (!isEmpty(readyQueue)){	
				//NOTE: could make code more efficient by sorting only when process is added to readyqueue. 
				//Better yet, add the process to appropriate index so it is already sorted. 
				//sortQueueByPriority is more constructive though (don't have to alter existing code).
				sortQueueByPriority(readyQueue); 
				if (partitions[0].runningProcess != NULL){
					if (readyQueue->head->pcb.priority < partitions[0].runningProcess->pcb.priority){
						writeTransition(tick, "RUNNING", "READY", partitions[0].runningProcess, outFilePtr);
						enqueue(readyQueue, partitions[0].runningProcess);

						partitions[0].runningProcess = dequeue(readyQueue);
						writeTransition(tick, "READY", "RUNNING", partitions[0].runningProcess, outFilePtr);
					} 
				} else { //handles the case for if there are multiple processes for tick 0, 
					partitions[0].runningProcess = dequeue(readyQueue);
					writeTransition(tick, "READY", "RUNNING", partitions[0].runningProcess, outFilePtr);

				}
		
			}	
		}

		//if round robin is enabled, check if processes can "round-robined"
		if (isRoundRobin){
			remainingRoundRobinTicks--;
			if (remainingRoundRobinTicks == 0){
				if (!isEmpty(readyQueue) && partitions[0].runningProcess != NULL){
					writeTransition(tick, "RUNNING", "READY", partitions[0].runningProcess, outFilePtr);
					enqueue(readyQueue, partitions[0].runningProcess);

					partitions[0].runningProcess = dequeue(readyQueue);
					writeTransition(tick, "READY", "RUNNING", partitions[0].runningProcess, outFilePtr);
				}
				remainingRoundRobinTicks = roundRobinTicks;
			}
		}
		
		//see if main CPU is idle and if there is a process waiting on the ready queue
		for (int i = 0; i < num_partitions; i++){
			if (partitions[i].runningProcess == NULL && !isEmpty(readyQueue)){

	

				if (partitions[i].memory >= front(readyQueue)->pcb.memory){ //first fit algo
					partitions[i].runningProcess = dequeue(readyQueue);
					if (partitions[i].runningProcess != NULL){
						writeTransition(tick, "READY", "RUNNING", partitions[i].runningProcess, outFilePtr);
						if (isMem)
							recordMem(partitions, num_partitions, &avg_mem_used, &avg_free_mem,  &avg_usable_mem, &n, outFilePtr);

						if (isRoundRobin)
							remainingRoundRobinTicks = roundRobinTicks; //for use in roundrobin
					}	
				}
			}
		}


		//ready queue wait time for use in analysis
		for (NODE* iter = readyQueue->head; iter!=NULL; iter = iter->prev){
			iter->pcb.wait_time++;
		}

	}


	analysis(terminatedQueue, tick-1, outFilePtr);
	if (isMem)
		analysisMem(&avg_mem_used, &avg_free_mem,  &avg_usable_mem, outFilePtr);

		
	fclose(outFilePtr);
	return 0;
}





