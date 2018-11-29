#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>
#include <queue>
using namespace std;

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

	Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
            master_burst = burst;
            age=0;
            waitingTime=0;
	}
    
    void increaseWaitingTime(){
        waitingTime++;
    }
};

vector<Process*> createProcesses(){
    vector<Process*> processes;

    string line;
//    ifstream myfile ("test_input.txt");
      ifstream myfile ("process_input.txt");
//          ifstream myfile ("test1k.txt");

    if (myfile.is_open()){
        getline(myfile, line); //remove first line
        
        while ( getline (myfile, line) ){
            istringstream iss(line); // make line a stream
            string pid, burst, arrival, priority, deadline, io;
            
            //get process input
            getline(iss, pid, '\t');
            getline(iss, burst, '\t');
            getline(iss, arrival, '\t');
            getline(iss, priority, '\t');
            getline(iss, deadline, '\t');
            getline(iss, io, '\t');
            
            //convert to int
            int pid_ = stoi(pid);
            int burst_ = stoi(burst);
            int arrival_ = stoi(arrival);
            int priority_ = stoi(priority);
            int deadline_ = stoi(deadline);
            int io_ = stoi(io);
            
            
            //sanitize input
            if(pid_ >= 0 &&
               burst_ >= 0 &&
               arrival_ >= 0 &&
               priority_ >= 0 &&
               deadline_ >= 0 &&
               io_ >= 0){
                
                //create process
                Process* proc = new Process(pid_, burst_, arrival_, priority_, deadline_, io_);
                processes.push_back(proc);
            }
            
        }
        myfile.close();
    }else{
        cout << "Unable to open file";
    }
    return processes;
}

bool compare(const Process* l, const Process* r){
    return l->arrival > r->arrival;
}

bool containProcesses(vector<Process*> processes, vector<queue<Process*> > vector_queues){
    bool result = false;
    
    for(int i = 0; i < vector_queues.size(); i++){
        if(vector_queues[i].size() > 0){
            result = true;
        }
    }
    
    return (processes.size() > 0) || result;
}

void age_FCFS(vector<queue<Process*> >& vector_queues, int age_interval, int amountToAge, fstream& fs){
    int queue_size = vector_queues[vector_queues.size()-1].size();
    
    while(queue_size > 0){// go through the queue to age
        Process* proc_front = vector_queues[vector_queues.size()-1].front(); //grab the last queue from all the queues
        vector_queues[vector_queues.size()-1].pop();
        proc_front->age += amountToAge;
        if(proc_front->age >= age_interval){ //age it up to the next queue
            proc_front->age = 0; //restart age
            vector_queues[vector_queues.size()-2].push(proc_front); //add to the queue above
//            fs << "Process " << proc_front->pid << " has aged and was promoted to queue " << vector_queues.size()-2 << "\n";
        }else{
            vector_queues[vector_queues.size()-1].push(proc_front);
        }
        
        queue_size--;
    }
}


void MFQS(vector<Process*> processes, fstream& fs){
    //an array of queues
    //with an index pointing to which queue we are currently at
    int queue_index = 0;
    queue<Process*> queue1;
    queue<Process*> queue2;
    queue<Process*> queue3;
    queue<Process*> queue4;
    
    vector<queue<Process*> > vector_queues;
    vector_queues.push_back(queue1);
    vector_queues.push_back(queue2);
    vector_queues.push_back(queue3);
    vector_queues.push_back(queue4);

    
    Process* cpu_process = NULL;
    
    int MFQS_timer = 0;
    const int master_time_quantum = 50;
    int master_queue_time_quantum = 0;
    int time_quantum = master_time_quantum;
    int age_interval = 25;
    int master_waitTime = 0;
    int master_completed = 0;
    int master_turnaround = 0;
    
    while( containProcesses(processes, vector_queues) ) {
        //gather new processes when their arrival comes
        while(processes.back() != NULL && processes.back()->arrival <= MFQS_timer){
            vector_queues[0].push( processes.back() );
            processes.pop_back();
        }
        
        queue_index = 0;
        //loop through the queues and stop at the first queue with a process
        while(queue_index < vector_queues.size() && vector_queues[queue_index].size() == 0){ //find a queue with a process
            queue_index++;
        }

        if(queue_index < vector_queues.size()){// need to check if we actually found a queue with processes or we ran over
            cpu_process = vector_queues[queue_index].front();//grab the first process from the queue
//            cout << "chosen pid: " << cpu_process->pid << "\n";
            
            time_quantum = pow(2, queue_index) * master_time_quantum; //reset time quantum for new process, for current queue
            master_queue_time_quantum = time_quantum;
        }else{
            //nothing was found
            
            MFQS_timer = processes.back()->arrival;
//            fs << MFQS_timer << "\n";
        }
        
        if(queue_index+1 == vector_queues.size()){ //if we are in the last one, dont do time quantum
            //do aging
            int amountToAge = cpu_process->burst;
            MFQS_timer += amountToAge; //time has elapsed
            
            master_completed++;
            cpu_process->waitingTime = (MFQS_timer - cpu_process->arrival) - cpu_process->master_burst;
            master_turnaround += cpu_process->waitingTime + cpu_process->master_burst;
            master_waitTime += cpu_process->waitingTime; //when the process dies add its waiting time
            
//            fs << "Process " << cpu_process->pid << " ran from " << MFQS_timer - (master_queue_time_quantum-amountToAge) << " through " << MFQS_timer-1 << " in queue " << queue_index << " and has finished\n";
            
            vector_queues[vector_queues.size()-1].pop();
            delete cpu_process;
            cpu_process = NULL;
            
            //age everything in the last queue
            age_FCFS(vector_queues, age_interval, amountToAge, fs);
            
            
        }else if(queue_index < vector_queues.size()){ // all queues before last queue
            //do time quantum
            
            int amountToAge = 0;
            if(cpu_process->burst <= time_quantum){
                amountToAge = cpu_process->burst;
                
                age_FCFS(vector_queues, age_interval, amountToAge, fs);//age everything in the last queue

                master_completed++;
                cpu_process->waitingTime = ((amountToAge + MFQS_timer) - cpu_process->arrival) - cpu_process->master_burst;
                master_turnaround += cpu_process->waitingTime + cpu_process->master_burst;
                master_waitTime += cpu_process->waitingTime; //when the process dies add its waiting time
                
//                fs << "Process " << cpu_process->pid << " ran from " << MFQS_timer << " through " << (amountToAge + MFQS_timer)-1 << " in queue " << queue_index << " and has finished\n";
                
                vector_queues[queue_index].pop();
                delete cpu_process;
                cpu_process = NULL;
            }else{
                amountToAge = time_quantum;
                
                cpu_process->burst -= amountToAge;
                
                age_FCFS(vector_queues, age_interval, amountToAge, fs);//age everything in the last queue
                
                    //demote process - took too long
                    Process* proc = cpu_process;
                    vector_queues[queue_index].pop();
                    vector_queues[queue_index+1].push( proc );//add the current process to the queue below
//                    fs << "Process " << proc->pid << " ran from " << (amountToAge+MFQS_timer)-master_queue_time_quantum << " through " << (amountToAge+MFQS_timer)-1 << " in queue " << queue_index << " and has been demoted to queue " << queue_index+1 << "\n";
                
            }
            
            MFQS_timer += amountToAge;//time has elapsed
        }
//        cout << MFQS_timer << "\n";
    }
    
    fs << "\nStats:\n";
    fs << "Total wait time: " << master_waitTime << "\n";
    fs << "Total completed: " << master_completed << "\n";
    fs << "Average turn around time: " << master_turnaround/master_completed << "\n";
    fs << "Average wait time: " << master_waitTime/master_completed << "\n";
}




int main(int argc, char * argv[]){
//    vector<Process*> processes = createProcesses();
//    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
    
    
    string read = "";
    
    fstream fs;
    fs.open("test.txt", std::fstream::in | std::fstream::out  | std::fstream::trunc );
    while(read != "Exit" && read != "5"){
        fs << flush; // print everything in the buffer
        cout << "Enter the number of the scheduling algorithm you would like to use?\n";
        cout << "\tMFQS: 1\n";
        cout << "\tRTS: 2\n";
        cout << "\tWHS: 3\n";
        cout << "\tCreate process: 4\n";
        cout << "\tExit: 5\n";
        cout << "Option: ";
        cin >> read;
    
    
        
        if(read == "1"){
            //MFQS
            vector<Process*> processes = createProcesses();
            sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
            
            MFQS(processes, fs);
        }
        
        cout << "\n";
        cout << "\n";
        
    }
     fs.close();

    return 0;
}






