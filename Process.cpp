#include "Process.h"

Process::Process(){
    
}

Process::Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
       master_burst = burst;
            
            age = 0;
            waitingTime = 0;
}

bool operator != (const Process& lhs, const Process& rhs){
     return lhs.pid < rhs.pid;
}

bool operator < (const Process& lhs, const Process& rhs){
    return lhs.priority < rhs.priority;
}

bool operator > (const Process& lhs, const Process& rhs){
    if(lhs.deadline == rhs.deadline){
        if(lhs.priority == rhs.priority){
            return lhs.pid > rhs.pid;//sort by lowest pid
        }else{
            return lhs.priority > rhs.priority; //sort by lowest priority
        }
    }
    return lhs.deadline > rhs.deadline; //sort by lowest priority
}




