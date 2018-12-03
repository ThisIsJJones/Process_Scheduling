#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>
#include <functional>   // std::greater
#include "Process.h"
#include "RTS.h"
using namespace std;

void RTS(vector<Process*> processes, fstream& fs){
    
    int RTS_TIMER = 0;
    
    bool haveProcess = false;
    int number_removed = 0;
    int executingTime = 0;
    
    unsigned long long int master_waitTime = 0;
    unsigned long long int master_completed = 0;
    unsigned long long int master_turnaround = 0;
    
    
    //order queue
    priority_queue<Process, vector<Process>, greater<vector<Process>::value_type> >  priority_queue;
    
    Process current_process;
    
    //    while(processes.back() != NULL && processes.back()->arrival <= RTS_TIMER){
    //        Process* proc = processes.back();
    //        priority_queue.push( *proc );
    //        delete proc;
    //        processes.pop_back();
    //    }
    //
    //    while (priority_queue.size() > 0) {
    //        cout << priority_queue.top().pid << " " << priority_queue.top().priority << " " <<  priority_queue.top().deadline << "\n";
    //        priority_queue.pop();
    //    }
    
    while(processes.size() != 0 || priority_queue.size() != 0){
        while(processes.back() != NULL && processes.back()->arrival == RTS_TIMER){
            Process* proc = processes.back();
            priority_queue.push( *proc );
            delete proc;
            processes.pop_back();
        }
        
        if(haveProcess){
            if(priority_queue.top() != current_process){//are we still the top process
                //grab the new process that has come in
                current_process = priority_queue.top();//grab earliest deadline
                executingTime=0;
            }
            priority_queue.pop();//remove
        }else if(priority_queue.size() > 0){
            //grab the top process
            executingTime=0;
            current_process = priority_queue.top(); //grab earliest deadline
            priority_queue.pop();//remove
            haveProcess = true;
        }else{
            //we don't have a process and there is nothing in the queue
            haveProcess = false;
        }
        
        while(haveProcess && (RTS_TIMER >= current_process.deadline)){
            haveProcess=false;
            number_removed++;
            
            master_waitTime += (RTS_TIMER - current_process.arrival) - current_process.burst;
            master_turnaround += (RTS_TIMER - current_process.arrival);
            
            fs << "\t" << current_process.pid << " removed due to having a deadline already passed or that will pass before its burst\n";

            if(haveProcess){
                if(priority_queue.top() != current_process){//are we still the top process
                    //grab the new process that has come in
                    current_process = priority_queue.top();//grab earliest deadline
                    executingTime=0;
                }
                priority_queue.pop();//remove
            }else if(priority_queue.size() > 0){
                //grab the top process
                executingTime=0;
                current_process = priority_queue.top(); //grab earliest deadline
                priority_queue.pop();//remove
                haveProcess = true;
            }else{
                //we don't have a process and there is nothing in the queue
                haveProcess = false;
            }
        }
        
        if(haveProcess){
            fs << "Executing process " << current_process.pid << " with burst " << current_process.burst << " and deadline " << current_process.deadline << " at clock tick " << RTS_TIMER << "\n";
            executingTime++;
            //we have a process
//            if(RTS_TIMER >= current_process.deadline
//               || (RTS_TIMER+current_process.burst) > current_process.deadline) {
//                //clock tick is already after the deadline
//                //don't do anything
//                haveProcess = false;
//                number_removed++;
//                fs << "\tRemoved due to having a deadline already passed or that will pass before its burst\n";
//            }else{
                current_process.burst--;
                if(current_process.burst == 0){
                    //process has finished
                    master_completed++;
                    master_waitTime += (RTS_TIMER - current_process.arrival) - current_process.burst;
                    master_turnaround += (RTS_TIMER - current_process.arrival);
                    
                    fs << "\tFinished from " << (RTS_TIMER-executingTime)+1 << " through " << RTS_TIMER << "\n";
                    haveProcess = false;
                }else{
                    //it didn't finish its burst so add it back to the queue
                    priority_queue.push(current_process);
                    fs << "\tDidn't Finish\n";
                }
//            }
            
        }
        
        RTS_TIMER++;
    }
    
    
    fs << "\nStats:\n";
    fs << "Total wait time: " << master_waitTime << "\n";
    fs << "Total completed: " << master_completed << "\n";
    fs << "Average turn around time: " << master_turnaround/master_completed << "\n";
    fs << "Average wait time: " << master_waitTime/master_completed << "\n";
    
}
