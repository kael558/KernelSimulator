#include "queue.h"

#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>  
#include <string.h>




/*
Creates the nodes consisting of a process which gets put into the initial queue
*/
NODE* makeNode(int PID, int arrival_time, int r_CPU_time, int IO_freq, int IO_duration, int priority)
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
Creates the inputProcessList which holds the processes until they reach the arrival time
*/
Queue* createInputProcessList(char * filename){
	Queue* inputProcessList = createQueue();
	FILE *fp;
	char str[100];

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Could not open file %s",filename);
        exit(EXIT_FAILURE);
	}
	
	int size = 6;
	int pInfo[size];

	while (fgets(str, 100, fp) != NULL){
		int i = 0;
		
		for (char* token = strtok(str, " "); token!=NULL; token=strtok(NULL, " ")){
			pInfo[i++] = atoi(token);
		}

		if (i == 3){ //-IO, -priority
			pInfo[3] = 999;
			pInfo[4] = 0;
			pInfo[5] = 0;
		} else if (i == 4){ //-IO, +priority
			pInfo[5] = pInfo[3];
			pInfo[3] = 999;
			pInfo[4] = 0;
		} else if (i == 5) { //+IO, -priority
			pInfo[5] = 0;
		}

		NODE* p = makeNode(pInfo[0], pInfo[1], pInfo[2], pInfo[3], pInfo[4], pInfo[5]);

		enqueue(inputProcessList, p);
	}
	fclose(fp);

	return inputProcessList;
}


/*
Checks if a process from the inputProcessList is ready to be placed into the queue
*/
void handleArrivalOfProcess(Queue* inputProcessList, Queue* readyQueue, int tick, FILE* outFilePtr){
	while (!isEmpty(inputProcessList) && inputProcessList->head->pcb.arrival_time == tick){
		enqueue(readyQueue, dequeue(inputProcessList));
	}
}

/*
Simulates a CPU tick which runs a singular process called runningProcess
*/
void runMainCPUTick(PCB* runningProcess){
	runningProcess->remaining_CPU_time--;
	runningProcess->remaining_IO_start_time--;
}

/*
Simulates the IO CPUs which process everything in the waitingQueue
*/
void runIOCPUTick(Queue* waitingQueue){
	for (NODE* iter = waitingQueue->head; iter!=NULL; iter = iter->prev){ //IO CPUs
		iter->pcb.remaining_IO_end_time--;
	}
}

/*
Calls the respective CPU tick functions
*/
void handleTick(NODE* runningProcess, Queue* waitingQueue){
	if (runningProcess != NULL){
		runMainCPUTick(&(runningProcess->pcb));
	}
	runIOCPUTick(waitingQueue);
}


/*
calculates gcd between two numbers using euclids algo
*/
int gcd(int n1, int n2){
	while(n1!=n2)
    {
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
void analysis(Queue* terminatedQueue, int totalTime, FILE* file_ptr){
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



int main(int argc, char *argv[]){ 

	char inputFile[40] = "input", outputFile[40]="outputPriority";

	bool isRoundRobin = false, isPriority = false;
	int roundRobinTicks = 0;
	int remainingRoundRobinTicks = 0;

	if (argc>1){
		strcat(inputFile, argv[1]);
		strcat(outputFile, argv[1]);
		
		if (strcmp(argv[2], "roundRobin") == 0){
			isRoundRobin = true;
			roundRobinTicks = atoi(argv[3]);
			remainingRoundRobinTicks = roundRobinTicks;
		} else if (strcmp(argv[2], "priority") == 0){
			isPriority = true;
		}
	} else { //for local execution
		char name[] = "CPUIOSharedBound.txt";
		strcat(inputFile, name);
		strcat(outputFile, name);
		//isRoundRobin = true;
		//roundRobinTicks = 100;
		//remainingRoundRobinTicks = roundRobinTicks;

		isPriority = true;
	}


	FILE* outFilePtr = fopen(outputFile, "w");

	Queue* inputProcessList = createInputProcessList(inputFile);
    Queue* readyQueue = createQueue(); 
	Queue* waitingQueue = createQueue(); 
	Queue* terminatedQueue = createQueue(); //for use in analysis
	NODE* runningProcess = NULL;
	sortQueueByArrivalTime(inputProcessList);

	int tick = 0;
	for (; !isEmpty(readyQueue) || !isEmpty(waitingQueue) || runningProcess!=NULL || !isEmpty(inputProcessList); tick++){
			
		handleArrivalOfProcess(inputProcessList, readyQueue, tick, outFilePtr);

		handleTick(runningProcess, waitingQueue);

		//CPU scheduler - see if process in waiting queue is finished processing
		sortQueueByRemainingIOEndTime(waitingQueue);
		while (!isEmpty(waitingQueue) && waitingQueue->head->pcb.remaining_IO_end_time == 0){
			waitingQueue->head->pcb.remaining_IO_start_time = waitingQueue->head->pcb.IO_start_time;
			writeTransition(tick, "WAITING", "READY", waitingQueue->head, outFilePtr);
			enqueue(readyQueue, dequeue(waitingQueue));
		}

		//see if current process is terminated or needs IO CPU
		if (runningProcess != NULL){
			if (runningProcess->pcb.remaining_CPU_time == 0){
				writeTransition(tick, "RUNNING", "TERMINATED", runningProcess, outFilePtr);
				enqueue(terminatedQueue, runningProcess);
				runningProcess = NULL;
			} else if (runningProcess->pcb.remaining_IO_start_time == 0){
				writeTransition(tick, "RUNNING", "WAITING", runningProcess, outFilePtr);
				runningProcess->pcb.remaining_IO_end_time = runningProcess->pcb.IO_end_time; 
				enqueue(waitingQueue, runningProcess);
				runningProcess = NULL;
			}
		}


		//if is priority is enabled, check if process in ready queue has higher priorirty
		if (isPriority){
			if (!isEmpty(readyQueue)){	
				//NOTE: could make code more efficient by sorting only when process is added to readyqueue. 
				//Better yet, add the process to appropriate index so it is already sorted. 
				//sortQueueByPriority is more constructive though (don't have to alter existing code).
				sortQueueByPriority(readyQueue); 

				if (runningProcess != NULL){
					if (readyQueue->head->pcb.priority < runningProcess->pcb.priority){
						writeTransition(tick, "RUNNING", "READY", runningProcess, outFilePtr);
						enqueue(readyQueue, runningProcess);

						runningProcess = dequeue(readyQueue);
						writeTransition(tick, "READY", "RUNNING", runningProcess, outFilePtr);
					} 
				} else { //handles the case for if there are multiple processes for tick 0, 
					runningProcess = dequeue(readyQueue);
					writeTransition(tick, "READY", "RUNNING", runningProcess, outFilePtr);

				}
			}	
		}


		//if round robin is enabled, check if processes can "round-robined"
		if (isRoundRobin){
			remainingRoundRobinTicks--;
			if (remainingRoundRobinTicks == 0){
				if (!isEmpty(readyQueue) && runningProcess != NULL){
					writeTransition(tick, "RUNNING", "READY", runningProcess, outFilePtr);
					enqueue(readyQueue, runningProcess);

					runningProcess = dequeue(readyQueue);
					writeTransition(tick, "READY", "RUNNING", runningProcess, outFilePtr);
				}
				remainingRoundRobinTicks = roundRobinTicks;

			}
		}

	
		//see if main CPU is idle and if there is a process waiting on the ready queue
		if (runningProcess == NULL){
			runningProcess = dequeue(readyQueue);
			if (runningProcess != NULL){
				writeTransition(tick, "READY", "RUNNING", runningProcess, outFilePtr);
				if (isRoundRobin)
					remainingRoundRobinTicks = roundRobinTicks; //for use in roundrobin
			}	
		}

		
		//ready queue wait time for use in analysis
		for (NODE* iter = readyQueue->head; iter!=NULL; iter = iter->prev){
			iter->pcb.wait_time++;
		}

		/*
		printf("tick%d\n", tick);
		printf("Running process:\n%i %i %i %i %i %i %i %i\n", 
			runningProcess->pcb.PID, 
			runningProcess->pcb.arrival_time, 
			runningProcess->pcb.remaining_CPU_time, 
			runningProcess->pcb.IO_start_time, 
			runningProcess->pcb.remaining_IO_start_time, 
			runningProcess->pcb.IO_end_time,
			runningProcess->pcb.remaining_IO_end_time, 
			runningProcess->pcb.priority);
		display(readyQueue, "ready");
		display(waitingQueue, "waiting");
		display(terminatedQueue, "terminated");

		if (tick > 3)
			exit(0);*/

	}

	analysis(terminatedQueue, tick-1, outFilePtr);


	fclose(outFilePtr);
	return 0;
}





