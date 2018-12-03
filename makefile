run : MainScheduler.cpp Process.o MFQS.o RTS.o WHS.o
	g++ -std=c++11 -o ./scheduler MainScheduler.cpp Process.o MFQS.o RTS.o WHS.o 

Process.o : Process.h Process.cpp
	g++ -c Process.h Process.cpp 

MFQS.o : MFQS.h MFQS.cpp
	g++ -c MFQS.h MFQS.cpp

RTS.o : RTS.h RTS.cpp
	g++ -c RTS.h RTS.cpp

WHS.o : WHS.h WHS.cpp
	g++ -c WHS.h WHS.cpp

clean:
	rm WHS.o
	rm RTS.o
	rm MFQS.o
	rm ./scheduler
	rm Process.o
	rm *.h.gch
	rm output.txt
