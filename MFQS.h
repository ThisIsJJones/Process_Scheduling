#ifndef MFQS_H
#define MFQS_H
#include "Process.h"
#include <vector>
#include <fstream>
using namespace std;

void MFQS(vector<Process*> processes, fstream& fs, int time_quantum_, int age_interval_, int nQueues_);

#endif
