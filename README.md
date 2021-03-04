**BAT FILES** <br />
test1.bat - simulates Part I. c). FCFS & Roundrobin and d). FCFS & Roundrobin - creates 4 output files <br />
test2.bat - simulates all 3 scheduling algo's with all 3 input files (CPU, IO, CPU & IO bound) - creates 9 output files <br />
test3.bat - simulates FCFS with 2 memory specs (mem1 & mem2 *see below) - creates 2 output files <br />
<br />
**INPUT** <br />
Input filenames : input* <br />
Input syntax: <br />
"PID arrival burst IO_freq IO_duration priority memory" <br />
*IO_freq, IO_duration, priority, memory all may be omitted if desired. <br />
<br />
Input examples: <br />
<br />
**OUTPUT** <br />
Output filenames : output *<br />
<br />
**MEMORY** <br />
Memory Partitions: <br />
Loads into the program the partitions of memory to be used in the simulation <br />
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





