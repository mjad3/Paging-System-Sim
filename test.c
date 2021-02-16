#define _GNU_SOURCE
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "replacement.h"



struct process {
  int pid;
  long int start;
  long int end;
  //last clock time - unsigned long int
  //const struct node * p;
  unsigned long int time; //store next runnable time
  int ready; //if waiting on io
  void * root;
};

struct process * makeProcess(char * pid, long int start) {
  struct process * e = (struct process * ) calloc(1, sizeof(struct process));
  e -> pid = atoi(pid);
  
  e -> start = start;
  e -> ready = 0;
  e -> time = 0;
  e -> root = NULL;
  return e;
}


//LRU
//read from file
//search list
//if full, remove
//if not, and not full read into end of list
//virutal wait/switch

//-get rid of end page, add new to end

int main(int argc, char * argv[]) {
  long int fileEnd;
  FILE * file = fopen(argv[1], "r");
  //int frames = 250; //update this with args
  //int size = 0; //number of full frames
  struct process * p_arr[50];
  //parse file
  char line[100];
  long int counter = 0;
  int spot = 0; //tracks number of PIDs
  int page_size = 4096; // Size of frame in Bytes
	int mem_size = 1; // Size of memory in MB
  int arg = 0;
	while ((arg = getopt(argc, argv, "pm")) != -1)	{
		switch(arg)	{
			case 'p':
          printf("%d", arg);
					page_size = arg;
				break;
			case 'm':
          printf("%d", arg);
					mem_size = arg;
				break;
		}
	}
  
  while (fgets(line, sizeof(line), file)) {
    counter++;
    long int lineStart = ftell(file) - strlen(line);
    //get pid, ignore spaces, add to struct
    //if not in table, set start
    //else update end

    char pid[10];
    int d = 0;
    int z = 0;
    char t;
    for (int x = 0; x < strlen(line); x++) {
      t = line[x];
      if (t == EOF || t == '\n') break;
      if ((t == ' ' || t == '\t') && z == 1) break;
      if (t != ' ' && t != '\t') {
        pid[d] = t;
        d++;
        z = 1;
      }
    }
    pid[d] = '\0';

    //pid = strtok(NULL, " ");
    //int vpn = atoi(pid);
    //printf("vpn: %d\n", vpn);
    int c = 0;

    for (int x = 0; x < spot; x++) {
      if (p_arr[x] -> pid == atoi(pid)) {
        c = 1;
        p_arr[x] -> end = lineStart;
        fileEnd = ftell(file);
      }
    }
    if (c != 1) {
      p_arr[spot] = makeProcess(pid, lineStart);
      spot++;
      c = 0;
    } else {}

  }
  long int AMU = 0;
  long int ARP = 0;
  
  long int TPI = 0;
  unsigned long int clock = 0; //global clock in ns
  for (int x = 0; x < spot; x++) {
    
    printf("pid: %d start: %ld end: %ld\n", p_arr[x] -> pid, p_arr[x] -> start, p_arr[x] -> end);

  }

  rewind(file);
  int lastPID;
  while (fgets(line, sizeof(line), file)) {
    long int lineStart = ftell(file) - strlen(line);
    //printf("traceline: %s\n", line);
    char * p = calloc(1, sizeof(char) * 10); //pid
    char * v = calloc(1, sizeof(char) * 10);  //vpn
    int d = 0;  //for keeping track of place of char in string
    int z = 0; //tracks seperation of pid and vpn
    char t;
    //parse pid and vpn of line
    for (int x = 0; x < strlen(line); x++) {
      t = line[x];
      if (t == EOF || t == '\n') break;
      if ((t == ' ' || t == '\t') && z == 1) {
        z++;
        d = 0;
      }
      if (t != ' ' && t != '\t') {

        if (z <= 1) {

          p[d] = t;
          d++;
          z = 1;
        } else if (z > 1) {
          v[d] = t;
          d++;
        }
      }
    }
    p[d] = '\0'; //do we need this?
    v[d] = '\0';
    int pid = atoi(p);
    int vpn = atoi(v);
    int runFail = 1;
    int attempt;
    //find matcching pid and run/fault
    for (int x = 0; x < spot; x++) {
      // If Line PID = Current Tree PID    
      if (p_arr[x] -> pid == pid) {
        attempt = pid; //store attempted index
        // And if the process is runnable
        //printf("id: %d, time: %ld, clock: %ld, start: %ld, ftell: %ld\n",pid, p_arr[x] -> time,clock, p_arr[x] -> start, lineStart);
        if (p_arr[x] -> time <= clock && p_arr[x] -> start >= lineStart) {
          clock++;
          
          runFail = 0;
          // Run the Process --> This passes the paramaters to our algorihtm
          int t = run(vpn, &(p_arr[x] -> root), p_arr[x]->ready);
          //update stats
          AMU += size;
          for (int j = 0; j < spot; j++) {
              if(p_arr[j]->time > clock) ARP++;
          }
          
          // If the process gets blocked (data structure is full and has to goto i.o)
          if (t == 1) {
            TPI++;
            //set new start point for process && set ready for io
            p_arr[x] -> start = lineStart;
            p_arr[x] -> ready = 1;
            
            //finds next available time slot in io queue
            long int maxTime = clock;
            for (int j = 0; j < spot; j++) {
              if (p_arr[j] -> time > maxTime) maxTime = p_arr[j] -> time;
            }
            p_arr[x] -> time = maxTime + 2000000; // increment time for io for this process
            
            
            
          }else { // no fault... does not have to increment i.o.
            p_arr[x] -> start = ftell(file);
            if (p_arr[x] -> end <= p_arr[x] -> start){
              
             //tdestroy(p_arr[x] -> root, free_node);
             deleteTree((p_arr[x] -> root));
            }
            p_arr[x]->ready = 0;
          }
          

        }
      }
    }
    
    
    //if line was invalid    
    if(attempt != lastPID && runFail && !(p_arr[lastPID] -> end <= p_arr[lastPID] -> start)){
      
      p_arr[lastPID]->start = ftell(file);
      if (p_arr[lastPID] -> end <= p_arr[lastPID] -> start){
        //p_arr[lastPID] = NULL;
        //spot--;
      }
      printf("line: %s\n", line);
      for (int x = 0; x < spot; x++) {
        printf("pid: %d, lastpid: %d time: %ld, clock: %ld, start: %ld fileEnd: %ld\n", attempt, lastPID, p_arr[x]->time, clock, p_arr[x]->start, fileEnd);
      }
    }
    //switch to runnable process, move line pointer to its start        
    long int min = fileEnd; //init minimum start point to eof for comparison
    int runnable = -1; // 
    int pTime; //index of process with next io time ending
    long int minTime = clock + 2000000; // sets min possible time of next io completion
     // For each tree
    for (int j = 0; j < spot; j++) {
      // If the process time is less than the next low time, set time slot to jump to
      
      // Or find earliest runnable process in trace file
      if (p_arr[j] -> time <= clock && p_arr[j] -> start < min) {
        min = p_arr[j] -> start;
        //printf("min: %ld\n", min);
        runnable = j;
      }else if (p_arr[j] -> time <= minTime) {
        minTime = p_arr[j] -> time;
        pTime = j;
      }
    } // If none are runnable
    if (runnable == -1) {
      AMU = AMU + (minTime - clock) * size;
      // increment clock, jump to process
      clock = minTime;
      
      lastPID = p_arr[pTime]->pid;
      fseek(file, p_arr[pTime] -> start, SEEK_SET);
    } else { 
      //printf("runnable: %d, clock: %ld\n", runnable, clock);     
      //next proc is p_arr[runnable];
      lastPID = p_arr[runnable]->pid;
      fseek(file, p_arr[runnable]->start, SEEK_SET);
    }
    
  }
  float a = (float)AMU/(clock * 100);
  float r = (float)ARP/clock;
  printf("AMU: %f, ARP: %f, TMR: %ld, TPI: %ld\n", a, r, counter, TPI);
  return 0;



}