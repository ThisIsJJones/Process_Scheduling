run : MainScheduler.cpp Process.o MFQS.o
	g++ -o ./scheduler MainScheduler.cpp MFQS.o Process.o

Process.o : Process.h Process.cpp
	g++ -c Process.h Process.cpp 

MFQS.o : MFQS.h MFQS.cpp
	g++ -c MFQS.h MFQS.cpp

clean:
	rm MFQS.o
	rm ./scheduler
	rm Process.o
	
