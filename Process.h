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
    Process();
    friend bool operator != (const Process& lhs, const Process& rhs);
    friend bool operator < (const Process& lhs, const Process& rhs);
    friend bool operator > (const Process& lhs, const Process& rhs);
};



#endif
