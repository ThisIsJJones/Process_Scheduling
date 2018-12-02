#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <string>
#include <queue>
#include "Process.h"
#include "MFQS.h"
#include "RTS.h"
#include "WHS.h"
using namespace std;

vector<Process*> createProcesses(){
    vector<Process*> processes;
    
    string line;
    //    ifstream myfile ("test_input.txt");
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
    string outputFileName = "output.txt";
    
    fstream fs;
    fs.open(outputFileName, std::fstream::in | std::fstream::out  | std::fstream::trunc );
    vector<Process*> createdProcesses;
    while(readInOption != "5"){
        fs << flush; // print everything in the buffer
        cout << "Enter the number of the scheduling algorithm you would like to use?\n";
        cout << "\tMFQS: 1\n";
        cout << "\tRTS: 2\n";
        cout << "\tWHS: 3\n";
        cout << "\tCreate process: 4\n";
        cout << "\tExit: 5\n";
        cout << "Option: ";
        cin >> readInOption;
        
        int option = -1;
        if(isInteger(readInOption)){
            option = stoi(readInOption);
        }
        
        vector<Process*> processes;
        
        switch(option){
            case 1:
                //MFQS
                processes = createProcesses();
                processes.insert(processes.end(), createdProcesses.begin(), createdProcesses.end());
                sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                
                MFQS(processes, fs);
                cout << "read output in file: " << outputFileName << "\n";
                break;
            case 2:
                //RTS
                processes = createProcesses();
                processes.push_back( createdProcesses.back() );
                //                processes.insert(processes.end(), createdProcesses.begin(), createdProcesses.end());
                sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                RTS(processes, fs);
                cout << "read output in file: " << outputFileName << "\n";
                break;
            case 3:
                //WHS
                processes = createProcesses();
                sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
                
                WHS(processes, fs);
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
    fs.close();
    
    return 0;
}






