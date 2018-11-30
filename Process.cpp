#include "Process.h"


Process::Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
       master_burst = burst;
            
            age = 0;
            waitingTime = 0;
}




