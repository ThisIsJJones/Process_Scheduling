run : MainScheduler.cpp Process.o MFQS.o RTS.o
	g++ -o ./scheduler MainScheduler.cpp Process.o MFQS.o RTS.o 

Process.o : Process.h Process.cpp
	g++ -c Process.h Process.cpp 

MFQS.o : MFQS.h MFQS.cpp
	g++ -c MFQS.h MFQS.cpp

RTS.o : RTS.h RTS.cpp
	g++ -c RTS.h RTS.cpp

clean:
	rm RTS.o
	rm MFQS.o
	rm ./scheduler
	rm Process.o
	rm *.h.gch
