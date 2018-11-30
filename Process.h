#ifndef PROCESS_H
#define PROCESS_H

class Process{

public:
	int pid;
    int master_burst;
	int burst;
	int arrival;
	int priority;
	int deadline;
	int io;
    int age;
    int waitingTime;

	Process(int pid, int burst, int arrival, int priority, int deadline, int io);
    
};



#endif
