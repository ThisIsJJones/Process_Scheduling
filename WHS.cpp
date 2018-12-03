#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>
#include <queue>
#include <algorithm>
#include "Process.h"
#include "WHS.h"
using namespace std;

void WHS(vector<Process*> processes, fstream& fs){
    
    int WHS_TIMER = 0;
    const int master_time_quantum = 50;
    int time_quantum = 50;
    int age_interval = 100;
    
    bool running = false;
    vector<deque<Process*> > process_deck;
    
    unsigned long long int master_waitTime = 0;
    unsigned long long int master_completed = 0;
    unsigned long long int master_turnaround = 0;
    
    //initialize deques
    for(int i = 0; i <= 99; i++){
        deque<Process*> deque_of_processes = deque<Process*>();
        process_deck.push_back( deque_of_processes );
    }
    
    queue<Process*> waiting_queue;
    Process* current_process;
    
    int process_in_queues = 0;
    int top_dawg = 0;
    int queue_index = 0;
    int aging_quantity = 0;
    
    while(processes.size() != 0 || process_in_queues != 0){
        cout << "clock tick "<< WHS_TIMER << "\n";
        while(processes.size() != 0 && processes.back()->arrival == WHS_TIMER){
            Process* proc = processes.back();
            process_deck[proc->priority].push_back( proc );
            processes.pop_back();
            process_in_queues++;
            if(proc->priority < 10){
                aging_quantity++;
                proc->queue_arrival = WHS_TIMER;//determine what time it entered aging queues
            }
            
            if(proc->priority > top_dawg){
                top_dawg = proc->priority;
            }
        }
        
        if(!running){
            //FIND THE QUEUE WE NEED TO BE IN
            queue_index = top_dawg;
            
            if(queue_index >= 0){
                //we have processes
                current_process = process_deck[queue_index].front(); //grab the process from that queue
                process_deck[queue_index].pop_front(); //remove
                time_quantum=master_time_quantum;
                running = true;
                //                fs << "Executing " << current_process->pid << " with priority " << current_process->priority << " with burst " << current_process->burst << " at clock tick " << WHS_TIMER << "\n";
            }
        }
        
        //HANDLE WAITING QUEUE OF IO PROCESSES
        if(waiting_queue.size() > 0){
            Process* process_on_front = waiting_queue.front();
            if(process_on_front->queue_arrival + process_on_front->io == WHS_TIMER-1){
                //promotion
                
                int newPriority = 0;
                
                if(process_on_front->priority <= 49){
                    //lower Band
                    if(process_on_front->priority + process_on_front->io >= 49){
                        //put into 49
                        newPriority = 49;
                    }else{
                        //calculate new io queue
                        newPriority = process_on_front->priority + process_on_front->io;
                        if(newPriority < 10){
                            aging_quantity++;
                            process_on_front->queue_arrival = WHS_TIMER;//determine what time it entered aging queues
                        }
                    }
                }else{
                    //upper band
                    if(process_on_front->priority + process_on_front->io >= 99){
                        //put into 99
                        newPriority = 99;
                    }else{
                        //calculate new io queue
                        newPriority = process_on_front->priority + process_on_front->io;
                    }
                }
                
                process_deck[newPriority].push_back(process_on_front);
                
                if(newPriority > top_dawg){
                    top_dawg = newPriority;
                }
                
                waiting_queue.pop();
                process_on_front->io = 0;//set io to zero because it has finished
            }
        }
        
        
        //AGE 10% of AGING QUEUES
        const int aging_queues = 100/10; //aging queues -> default bottom 10%
        
        //HANDLE 10% OF AGING QUEUES
        for(int i = 0; i < aging_queues && aging_quantity > 0; i++){ //go through all aging queues

            if(process_deck[i].size() > 0){
                Process* process_on_front = process_deck[i].front(); //grab the first one

                while(process_deck[i].size() > 0 && process_on_front->queue_arrival + age_interval >= WHS_TIMER){ //is the age more than the interval
                    //needs to age
                    //PROMOTION
                    if(!running || (process_on_front->pid != current_process->pid)){//don't age the process we are running
                        process_on_front->priority = process_on_front->priority + aging_queues; //age up by how many aging queues there are

                        process_deck[process_on_front->priority].push_back(process_on_front);//add it to appropriate queue
                        
                        process_on_front->age = 0;
                        process_deck[i].pop_front();
                        aging_quantity--;
                        if(process_on_front->priority > top_dawg){
                            top_dawg = process_on_front->priority;
                        }
                        process_on_front = process_deck[i].front(); //grab the next process to check age

                    }
                }
            }
        }
        
        if(running){
            if(time_quantum==1 && current_process->io > 0){
                //we are on the last quantum
                //do I/O
                if(current_process->priority < 10){
                    aging_quantity--;
                }
                current_process->queue_arrival = WHS_TIMER;
                waiting_queue.push(current_process);
                running = false;
                //                fs << "\tDoing IO\n";
            }else{
                current_process->burst--; //execute
                //                fs << "\tburst: " << current_process->burst << "\n";
                
                if(current_process->burst==0){
                    //we dead
                    running = false;
                    
                    master_waitTime += (WHS_TIMER - current_process->arrival) - current_process->master_burst;
                    master_turnaround += (WHS_TIMER - current_process->arrival);
                    master_completed++;
                    if(current_process->priority < 10){
                        aging_quantity--;
                    }
                    delete current_process;
                    current_process = NULL;
                    process_in_queues--;
                }else if(time_quantum==0){
                    //time quantum expired
                    //demote
                    //                    fs << "\tWe got demoted to: ";
                    int newPriority = current_process->priority - master_time_quantum;
                    if(newPriority < current_process->master_priority){//check where process needs to go
                        newPriority = current_process->master_priority;
                        //                        fs << current_process->priority - master_time_quantum<<"\n";
                    }
                    process_deck[newPriority].push_back(current_process);
                    current_process->priority = newPriority;
                    if(newPriority < 10){
                        current_process->queue_arrival = WHS_TIMER;
                        aging_quantity++;
                        current_process->age = 0;
                    }
                    running = false;
                }else{
                    //                     fs << "\tstill running\n";
                    //we are still running
                    //we already have current process so keep going
                    running = true;
                }
            }
            
            if(!running){
                if(process_deck[queue_index].size() <= 0){
                    //we might need to change the top dawg
                    if(queue_index == top_dawg){
                        queue_index--;
                        while(queue_index >= 0 && process_deck[queue_index].size() == 0){
                            queue_index--;
                        }
                        top_dawg = queue_index;
                    }
                }
            }
            time_quantum--;
        }
        WHS_TIMER++;
    }
    
    fs << "\nStats:\n";
    fs << "Total wait time: " << master_waitTime << "\n";
    fs << "Total completed: " << master_completed << "\n";
    fs << "Average turn around time: " << master_turnaround/master_completed << "\n";
    fs << "Average wait time: " << master_waitTime/master_completed << "\n";
    
}











