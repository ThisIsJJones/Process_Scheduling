
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>
#include <queue>
#include <algorithm>
#include "Process.h"
#include "MFQS.h"
using namespace std;



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
    const int master_time_quantum = 100;
    int master_queue_time_quantum = 0;
    int time_quantum = master_time_quantum;
    int age_interval = 25;
    unsigned long long int master_waitTime = 0;
    unsigned long long int master_completed = 0;
    unsigned long long int master_turnaround = 0;
    int ready_processes = 0;

    while( ready_processes > 0 || processes.size()>0)  {
        //gather new processes when their arrival comes
        while(processes.size() > 0 && processes.back()->arrival <= MFQS_timer){
            vector_queues[0].push( processes.back() );
            processes.pop_back();
            ready_processes++;
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

            //fs << "Process " << cpu_process->pid << " ran from " << MFQS_timer - (master_queue_time_quantum-amountToAge) << " through " << MFQS_timer-1 << " in queue " << queue_index << " and has finished\n";

            vector_queues[vector_queues.size()-1].pop();
            delete cpu_process;
            cpu_process = NULL;
            ready_processes--;
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
                ready_processes--;
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
        cout << MFQS_timer << "\n";
    }

    fs << "\nStats:\n";
    fs << "Total wait time: " << master_waitTime << "\n";
    fs << "Total completed: " << master_completed << "\n";
    fs << "Average turn around time: " << master_turnaround/master_completed << "\n";
    fs << "Average wait time: " << master_waitTime/master_completed << "\n";
}

