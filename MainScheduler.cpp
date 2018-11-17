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
	int burst;
	int arrival;
	int priority;
	int deadline;
	int io;
    int age=0;

	Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
	
	}
    
    int currentAge(){
        return age;
    }
    
    void increaseAge(){
        age++;
    }
};

vector<Process*> createProcesses(){
    vector<Process*> processes;

    string line;
    ifstream myfile ("process_input.txt");
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

int main(int argc, char * argv[]){
    vector<Process*> processes = createProcesses();
    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
    
    //MFQS
    
    //an array of queues?
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
    int age_interval = 10;
    
    while(processes.size() > 0) {
        
        //gather new processes when their arrival comes
        while(processes.back()->arrival == MFQS_timer){
            vector_queues[0].push( processes.back() );
            processes.pop_back();
        }

        if(cpu_process == NULL || cpu_process->burst==0){//we are ready to grab a new process
            if(cpu_process != NULL && cpu_process->burst==0){
                delete cpu_process;
            }
            
            queue_index = 0;

            //loop through the queues and stop at the first queue with a process
            while(queue_index < vector_queues.size() && vector_queues[queue_index].size() == 0){ //find a queue with a process
                queue_index++;
            }

            if(queue_index < vector_queues.size()){// need to check if we actually found a queue with processes or we ran over
                cpu_process = vector_queues[queue_index].front();
                vector_queues[queue_index].pop();//grab the first process from the queue
                cpu_process->burst--;//decrease burst for process
                time_quantum = pow(2, queue_index) * master_time_quantum; //reset time quantum for new process, regarding queue index
            }
            
        }else{
            cpu_process->burst--;//decrease burst for process
        }
        
        if(queue_index+1 == vector_queues.size()){ //use aging if we are in the last one, else do time quantum
            //do aging
            queue<Process*> temp_queue;
            while(vector_queues[queue_index].size() > 0){ // go through the queue to age
                Process* proc = vector_queues[queue_index].front();
                proc->increaseAge();
                
                if(proc->age >= age_interval){ //age it up to the next queue
                    proc->age = 0;
                    vector_queues[queue_index-1].push(proc); //add to the queue above
                }
            
                vector_queues[queue_index].pop();//remove from this queue
                
                temp_queue.push(proc); // add process to temp queue
            }
            vector_queues.pop_back();
            vector_queues.push_back( temp_queue );
            
        }else if(queue_index < vector_queues.size()){ // all queues before last queue
            //do time quantum
            time_quantum--;//decrease time quantum
            
            if(time_quantum == 0){//time quantum has expired
                vector_queues[queue_index+1].push( cpu_process );//add the current process to the queue below
                cpu_process=NULL; //remove process so we can grab a new one
            }
        }
        
        MFQS_timer++;//increase clock tick
    }
    
    
    return 0;
}



