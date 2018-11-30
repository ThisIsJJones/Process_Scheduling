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
        }else if(read == "2"){
            //RTS
            vector<Process*> processes = createProcesses();
            sort(processes.begin(), processes.end(), compare); //sort the processes by arrival, last is 0
            RTS(processes, fs);
        }
        
        cout << "\n";
        cout << "\n";
        
    }
     fs.close();

    return 0;
}






