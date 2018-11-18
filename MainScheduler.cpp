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
    int age=0;
    int waitingTime=0;

	Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
            master_burst = burst;
	}
    
    void increaseAge(){
        age++;
    }
    
    void increaseWaitingTime(){
        waitingTime++;
    }
};

vector<Process*> createProcesses(){
    vector<Process*> processes;

    string line;
    ifstream myfile ("test_input.txt");
//      ifstream myfile ("process_input.txt");

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

void age_FCFS(vector<queue<Process*> >& vector_queues, int queue_index, int queue_size, int age_interval){
    while(queue_size > 0){// go through the queue to age
        Process* proc_front = vector_queues[queue_index].front();
        vector_queues[queue_index].pop();
        proc_front->increaseAge();
        if(proc_front->age >= age_interval){ //age it up to the next queue
            proc_front->age = 0; //restart age
            vector_queues[queue_index-1].push(proc_front); //add to the queue above
        }else{
            vector_queues[queue_index].push(proc_front);
        }
        
        queue_size--;
    }
}

void increase_allWaitingTimes(vector<queue<Process*> >& vector_queues, Process* current_process){
    for(int i = 0; i<vector_queues.size(); i++){
        
        int queue_size = vector_queues[i].size();
        while(queue_size > 0){// go through the queues
            
            Process* proc_front = vector_queues[i].front();
            vector_queues[i].pop();
            
            if((current_process != NULL && proc_front->pid != current_process->pid) || current_process == NULL ){
                proc_front->increaseWaitingTime();
            }
            
            vector_queues[i].push(proc_front);
            queue_size--;
        }
    }
}




int main(int argc, char * argv[]){
    vector<Process*> processes = createProcesses();
    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
    
    //MFQS
    
    //an array of queues
    //with an index pointing to which queue we are currently at
    queue<Process*> queue1;
    queue<Process*> queue2;
    queue<Process*> queue3;
    
    int queue_index = 0;
    vector<queue<Process*> > vector_queues;
    vector_queues.push_back(queue1);
    vector_queues.push_back(queue2);
    vector_queues.push_back(queue3);

    Process* cpu_process = NULL;
    
    int MFQS_timer = 0;
    const int master_time_quantum = 5;
    int time_quantum = master_time_quantum;
    int age_interval = 5;
    int master_waitTime = 0;
    int master_completed = 0;
    int master_turnaround = 0;
    bool tqexpired = true;
    
    while( containProcesses(processes, vector_queues) ) {
        cout << "CLOCK: " << MFQS_timer << "\n";
        //gather new processes when their arrival comes
        while(processes.back() != NULL && processes.back()->arrival == MFQS_timer){
            vector_queues[0].push( processes.back() );
            processes.pop_back();
        }
        cout << "tqe: " << tqexpired << "\n";
        if(tqexpired || (cpu_process != NULL && cpu_process->burst==0)){//we are ready to grab a new process
            cout << "here\n";

            if(cpu_process != NULL && cpu_process->burst==0){
                master_completed++;
                master_turnaround += cpu_process->waitingTime + cpu_process->master_burst;
                master_waitTime += cpu_process->waitingTime; //when the process dies add its waiting time
                vector_queues[queue_index].pop(); //remove process we just processed
                tqexpired = true;
                delete cpu_process;
                cpu_process = NULL; // uh oh
            }
            
            queue_index = 0;

            //loop through the queues and stop at the first queue with a process
            while(queue_index < vector_queues.size() && vector_queues[queue_index].size() == 0){ //find a queue with a process
                queue_index++;
            }
            cout << "chosen index: " << queue_index << " size: " << vector_queues[queue_index].size() <<"\n";
            if(queue_index < vector_queues.size()){// need to check if we actually found a queue with processes or we ran over
                cpu_process = vector_queues[queue_index].front();//grab the first process from the queue
                cout << "chosen pid: " << cpu_process->pid << "\n";
                cpu_process->burst--;//decrease burst for process
                tqexpired = false;
                time_quantum = pow(2, queue_index) * master_time_quantum; //reset time quantum for new process, for current queue
            }
            
        }else{
            cpu_process->burst--;//decrease burst for process
        }
        
        if(queue_index+1 == vector_queues.size()){ //use aging if we are in the last one, else do time quantum
            //do aging
            queue<Process*> temp_queue;

            //remove the first process
            Process* proc_front = vector_queues[queue_index].front();
            vector_queues[queue_index].pop();
            vector_queues[queue_index].push(proc_front);
            
            int queue_size = vector_queues[queue_index].size()-1; //the size of all the ages to increase
            age_FCFS(vector_queues, queue_index, queue_size, age_interval);//age everything in the last queue
            
        }else if(queue_index < vector_queues.size()){ // all queues before last queue
            //do time quantum
            time_quantum--;//decrease time quantum
            
            int queue_size = vector_queues[vector_queues.size()-1].size();//size of the last queue (FCFS)
            age_FCFS(vector_queues, vector_queues.size()-1, queue_size, age_interval);//age everything in the last queue
            
            if(time_quantum == 0){//time quantum has expired
                tqexpired = true;
                if(cpu_process->burst != 0){ //did this process finish at the time quantum?
                    Process* proc = cpu_process;
                    vector_queues[queue_index].pop();
                    vector_queues[queue_index+1].push( proc );//add the current process to the queue below
//                    cpu_process=NULL; //remove process so we can grab a new one
                }else{
                    //let process get deleted on next clock tick
                }
            }
        }
        increase_allWaitingTimes(vector_queues, cpu_process);
        MFQS_timer++;//increase clock tick
    }
    
    cout << "\nStats:\n";
    cout << "Total wait time: " << master_waitTime << "\n";
    cout << "Total completed: " << master_completed << "\n";
    cout << "Average turn around time: " << master_turnaround/master_completed << "\n";
    cout << "Average wait time: " << master_waitTime/master_completed << "\n";

    return 0;
}



