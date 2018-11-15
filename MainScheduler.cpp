#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

class Process{
	public:
	int pid;
	int burst;
	int arrival;
	int priority;
	int deadline;
	int io;

	Process(int pid, int burst, int arrival, int priority, int deadline, int io)
		: pid(pid), burst(burst), arrival(arrival), priority(priority), deadline(deadline), io(io){
	
	} 
};



int main(){

	string line;
  	ifstream myfile ("process_input.txt");
  	if (myfile.is_open()){
		getline(myfile, line); //remove first line
    		while ( getline (myfile, line) ){ 
                istringstream iss(line);
                string pid, burst, arrival, priority, deadline, io;
                getline(iss, pid, ' ');
                getline(iss, burst, ' ');
                getline(iss, arrival, ' ');
                getline(iss, priority, ' ');
                getline(iss, deadline, ' ');
                getline(iss, io, ' ');
                Process* proc = new Process(stoi(pid), stoi(burst), stoi(arrival), stoi(priority), stoi(deadline), stoi(io));
    		}
    		myfile.close();
  	}else 
		cout << "Unable to open file"; 

  return 0;
}



