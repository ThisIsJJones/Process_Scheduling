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
#include "RTS.h"
#include "WHS.h"
using namespace std;

vector<Process*> createProcesses(string filename){
    vector<Process*> processes;
    
    string line;
    //    ifstream myfile ("test_input.txt");
    //    ifstream myfile ("process_input.txt");
    ifstream myfile(filename);
    
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
        cout << "Unable to open file\n";
    }
    return processes;
}

bool compare(const Process* l, const Process* r){
    return l->arrival > r->arrival;
}

inline bool isInteger(const string& s){
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;
    
    char * p;
    strtol(s.c_str(), &p, 10);
    
    return (*p == 0);
}


int main(int argc, char * argv[]){
    //    vector<Process*> processes = createProcesses();
    //    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
    
    
    string readInOption = "";
    string mfqs_output_fileName = "mfqs_output.txt";
    string rts_output_fileName = "rts_output.txt";
    string whs_output_fileName = "whs_output.txt";
    
    fstream mfqs_output_file;
    fstream rts_output_file;
    fstream whs_output_file;
    
    
    cout << "\n\nWELCOME TO THE PROCESS SCHEDULER!\n\n";
    
    vector<Process*> createdProcesses;
    while(readInOption != "5"){
        
        cout << "Enter the number of the scheduling algorithm you would like to use?\n";
        cout << "\tMFQS: 1\n";
        cout << "\tRTS: 2\n";
        cout << "\tWHS: 3\n";
        cout << "\tCreate process: 4\n";
        cout << "\tExit: 5\n";
        cout << "Option: ";
        cin >> readInOption;
        cout << "\n";
        int option = -1;
        if(isInteger(readInOption)){
            option = stoi(readInOption);
        }
        
        vector<Process*> processes;
        
        switch(option){
            case 1: {
                //MFQS
                
                
                string file_creation = "";
                
                cout << "What input should we use?\n";
                cout << "filename: ";
                cin >> file_creation;
                
                processes = createProcesses(file_creation);
                if(processes.size() > 0){
                    for(int i = 0; i < createdProcesses.size(); i++){
                        Process OG_PROC = *(createdProcesses[i]);
                        Process* proc = new Process(OG_PROC.pid, OG_PROC.burst, OG_PROC.arrival, OG_PROC.priority, OG_PROC.deadline, OG_PROC.io);
                        processes.push_back( proc );
                    }
                    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                    
                    
                    cout << "Starting MFQS...\n";
                    
                    string time_quantum = "";
                    string nQueues = "";
                    string age_interval = "";
                    
                    cout << "We need some info first:\n";
                    cout << "Time quantum: ";
                    cin >> time_quantum;
                    cout << "Number of Queues: ";
                    cin >> nQueues;
                    cout << "Age Interval: ";
                    cin >> age_interval;
                    
                    if(isInteger(time_quantum) &&
                       stoi(time_quantum) > 0 &&
                       isInteger(nQueues) &&
                       stoi(nQueues) <= 5 &&
                       stoi(nQueues) > 0 &&
                       isInteger(age_interval) &&
                       stoi(age_interval) > 0){
                        
                        mfqs_output_file.open(mfqs_output_fileName, std::fstream::out  | std::fstream::trunc );
                        MFQS(processes, mfqs_output_file, stoi(time_quantum), stoi(age_interval), stoi(nQueues));
                        mfqs_output_file << "\n";
                        cout << "read output in file: " << mfqs_output_fileName << "\n";
                        mfqs_output_file << flush; // print everything in the buffer
                        mfqs_output_file.close();
                    }else{
                        cout << "Invalid input.\n";
                    }
                }
                
                break;
            }
                break;
            case 2: {
                //RTS
                string file_creation = "";
                
                cout << "What input should we use?\n";
                cout << "filename: ";
                cin >> file_creation;
                
                processes = createProcesses(file_creation);
                if(processes.size() > 0){
                    for(int i = 0; i < createdProcesses.size(); i++){
                        Process OG_PROC = *(createdProcesses[i]);
                        Process* proc = new Process(OG_PROC.pid, OG_PROC.burst, OG_PROC.arrival, OG_PROC.priority, OG_PROC.deadline, OG_PROC.io);
                        processes.push_back( proc );
                    }
                    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                    
                    cout << "Starting RTS...\n";
                    
                    rts_output_file.open(rts_output_fileName, std::fstream::out  | std::fstream::trunc );
                    
                    
                    RTS(processes, rts_output_file);
                    cout << "read output in file: " << rts_output_fileName << "\n";
                    rts_output_file << "\n";
                    rts_output_file << flush; // print everything in the buffer
                    rts_output_file.close();
                }
                
                break;
            }
                break;
            case 3: {
                //WHS
                string file_creation = "";
                
                cout << "What input should we use?\n";
                cout << "filename: ";
                cin >> file_creation;
                
                processes = createProcesses(file_creation);
                if(processes.size() > 0){
                    for(int i = 0; i < createdProcesses.size(); i++){
                        Process OG_PROC = *(createdProcesses[i]);
                        Process* proc = new Process(OG_PROC.pid, OG_PROC.burst, OG_PROC.arrival, OG_PROC.priority, OG_PROC.deadline, OG_PROC.io);
                        processes.push_back( proc );
                    }
                    sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                    
                    whs_output_file.open(whs_output_fileName, std::fstream::out  | std::fstream::trunc );
                    
                    cout << "Starting WHS...\n";
                    
                    string time_quantum = "";
                    string age_interval = "";
                    
                    cout << "We need some info first:\n";
                    cout << "Time quantum: ";
                    cin >> time_quantum;
                    cout << "Age Interval: ";
                    cin >> age_interval;
                    
                    if(isInteger(time_quantum) &&
                       stoi(time_quantum) > 0 &&
                       isInteger(age_interval) &&
                       stoi(age_interval) > 0){
                        WHS(processes, whs_output_file, stoi(time_quantum), stoi(age_interval));
                        cout << "read output in file: " << whs_output_fileName << "\n";
                        whs_output_file << "\n";
                        whs_output_file << flush; // print everything in the buffer
                        whs_output_file.close();
                    }
                }
                break;
            }
                break;
            case 4: {
                //CREATE PROCESS
                string pidS, burstS, arrivalS, priorityS, deadlineS, ioS;
                
                cout << "Enter in pid, burst, arrival, priority, deadline, and io for the process:\n";
                cout << "PID: ";
                cin >> pidS;
                cout << "Burst: ";
                cin >> burstS;
                cout << "Arrival: ";
                cin >> arrivalS;
                cout << "Priority: ";
                cin >> priorityS;
                cout << "Deadline: ";
                cin >> deadlineS;
                cout << "I/O: ";
                cin >> ioS;
                
                if(isInteger(pidS) &&
                   isInteger(burstS) &&
                   isInteger(arrivalS) &&
                   isInteger(priorityS) &&
                   isInteger(deadlineS) &&
                   isInteger(ioS)){
                    
                    //convert to int
                    int pidI_ = stoi(pidS);
                    int burstI_ = stoi(burstS);
                    int arrivalI_ = stoi(arrivalS);
                    int priorityI_ = stoi(priorityS);
                    int deadlineI_ = stoi(deadlineS);
                    int ioI_ = stoi(ioS);
                    
                    Process* processToAdd = new Process(pidI_, burstI_, arrivalI_, priorityI_, deadlineI_, ioI_);
                    createdProcesses.push_back(processToAdd);
                }
                break;
            }
                break;
            case 5:
                //exit
                break;
            default:
                cout << "\nPlease enter an option: 1, 2, 3, 4, or 5";
                break;
        }
        cout << "\n";
    }
    
    return 0;
}






