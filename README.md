To see this readMe without all these lovely line breaks, go to the repo: https://github.com/kael558/KernelSimulator <br /> <br />

Note: Accidently submitted ide specific files. Files necessary to run bat files are: <br />
* KernelSimulator.exe
* cpu_scheduler.c
* queue.c
* queue.h
* structs.h
* input folders
* output folders

<br /> This program simulates: <br />
1.) Scheduling algorithms - FCFS, RoundRobin & Priority. <br />
2.) Memory Management* - two different sets of partitions <br />
<br />
*NOTE: It was not explicity stated in the assignment 2 document that a process remains in memory while doing IO. Therefore, I assume that IO has it's own memory, similar to a disk, as in the first assignment. So when a process is doing IO, it allows another process to run in the partition that it left. <br />
<br />
**Bat Files** <br />
test1.bat - simulates Part I. c). FCFS & Roundrobin and d). FCFS & Roundrobin - creates 4 output files <br />
test2.bat - simulates all 3 scheduling algo's with all 6 input files (CPU, IO, CPU & IO bound 1 & 2) - creates 18 output files <br />
test3.bat - simulates FCFS with 2 memory specs (mem1 & mem2 *see below) with all 5 input files - creates 12 output files <br />

<br />
**Input** <br />
Input filenames : input_____ <br />
Input syntax: <br />
"PID arrival burst IO_freq IO_duration priority memory" <br />
*IO_freq, IO_duration, priority, memory all may be omitted if desired. <br />
<br />
Input examples: 
1 0 20 4 1 1 200 //priority 1, memory req 200<br />
2 0 20 4 1 2 250 //priority 2, memory req 250<br />
<br />
1 0 20 1 200 //no IO, priority 1, memory req 200<br />
2 0 20 2 250 //no IO, priority 2, memory req 250<br />
<br />

**Output** <br />
Output filenames : output_____ <br />
Output types: <br />
1.) Transition - tick | pid | old_state | new_state <br />
<br />
**Memory** <br />
Memory Partitions: <br />
TODO: have files containing partition info which can be specified on program run. E.g. load in mem1.txt which loads in those partition memory sizes<br />
Tells the program which hardcoded partitions of memory to be used in the simulation <br />
mem1 - 1 partition of "infinite" memory <br />
mem2 - 4 partitions of 500, 250, 150, 100 (causes memory to be output)<br />
mem3 - 4 partitions of 300, 300, 350, 50 (causes memory to be output)<br /> 
<br />
**USAGE** <br />
Usage syntax: <br />
"KernelSimulator inputFile hasIO hasPriority hasMemory outputFile MemorySpecs SchedulingAlgo" <br />
<br />
Usage examples: <br />
"KernelSimulator input_CPUBound.txt false true true output_FCFS_CPUBound.txt mem1 FCFS" <br />
Process input from input_CPUBound.txt. <br />
Input does not contain data for IO but does contain data for priority and memory. <br />
Simulates using FCFS & mem1 specifications. <br />
Output into output_FCFS_CPUBound.txt <br />





