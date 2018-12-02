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
    queue<Process*> waiting_queue;
    Process* current_process;
    
    int process_in_queues = 0;
    
    while(processes.size() != 0 || process_deck[99].size() != 0){
        while(processes.back() != NULL && processes.back()->arrival == WHS_TIMER){
            Process* proc = processes.back();
            process_deck[proc->priority].push_back( proc );
            process_in_queues++;
            delete proc;
            processes.pop_back();
        }
        
        if(!running){
            //FIND THE QUEUE WE NEED TO BE IN
            int queue_index = 99;
            while(process_deck[queue_index].size() == 0){
                queue_index--;
            }
            
            if(queue_index >= 0){
                //we have processes
                current_process = process_deck[queue_index].front(); //grab the process from that queue
                process_deck[queue_index].pop_front(); //remove
                time_quantum=master_time_quantum;
                running = true;
            }
        }
        
        if(time_quantum==1){
            //we are on the last quantum
            if(current_process->io > 0){
                //do I/O
                current_process->queue_arrival = WHS_TIMER;
                waiting_queue.push(current_process);
                running = false;
            }
        }else{
            current_process->burst--; //execute
            if(current_process->burst==0){
                //we dead
                running = false;
                process_in_queues--;
            }else if(time_quantum==0){
                //time quantum expired
                //demote
                if(current_process->priority-master_time_quantum >= current_process->master_priority){
                    //put into queue below
                    process_deck[current_process->priority-master_time_quantum].push_back(current_process);
                }else{
                    //put into OG queue
                    process_deck[current_process->master_priority].push_back(current_process);
                }
                running = false;
            }else{
                //we are still running
                running = true;
            }
        }
        
        
        //HANDLE WAITING QUEUE OF IO PROCESSES
        Process* processOnFront = waiting_queue.front();
        if(processOnFront->queue_arrival + processOnFront->io >= WHS_TIMER){
            //promotion
            if(processOnFront->priority <= 49){
                //lower Band
                if(processOnFront->priority + processOnFront->io >= 49){
                    //put into 49
                    process_deck[49].push_back(processOnFront);
                }else{
                    //calculate new io queue
                    process_deck[processOnFront->priority + processOnFront->io].push_back(processOnFront);
                }
            }else{
                //upper band
                if(processOnFront->priority + processOnFront->io >= 99){
                    //put into 99
                    process_deck[99].push_back(processOnFront);
                }else{
                    //calculate new io queue
                    process_deck[processOnFront->priority + processOnFront->io].push_back(processOnFront);
                }
            }
        }
        
        time_quantum--;
        WHS_TIMER++;
    }
    
}











