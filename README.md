Input files : input*

Output files : output *

Memory Partitions:
Loads into the program the partitions of memory to be used in the simulation
memory1 - 1 partition of "infinite" memory
memory2 - 4 partitions of 500, 250, 150, 100
memory3 - 4 partitions of 300, 300, 350, 50

Usage syntax:
"KernelSimulator inputFile outputFile MemorySpecs SchedulingAlgo"

Usage examples:
"KernelSimulator input_CPUBound.txt output_FCFS_CPUBound.txt memory1.txt FCFS"
Process input from input_CPUBound.txt.
Simulates using FCFS & memory1 specifications.
Output into output_FCFS_CPUBound.txt

