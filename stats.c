#include <stdlib.h>

// Average Memory Utilization (AMU):
// For each clock tick, examine how many pages frames are occupied and average this over each clock tick that the simulator runs.
//Average Runable Processes (ARP):
//This value is an average of the number of processes that are running (or runable). This value is averaged over each clock tick for which the simulator runs.
//Running Time:
//This is the total number of clock ticks for the simulator run.

// Clock ticks
int clock = 0;
//This is simply a count of the total number of memory references in the trace file.
int references = 0;
//This is the total number of page faults (resulting in disk transfers into memory).
int faults = 0;



// Increments 1 ns on successful read
// Increments 2 ms when it has to read from memory
int increment_clock(int increment)	{
	clock++;
	return clock;
}

int increment_reference()	{
	references++;
	return references;
}

int incremen_faults()	{
	faults++;
	return faults;
}

void print_stats()	{

}