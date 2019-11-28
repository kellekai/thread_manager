main: FTthreadManager.o
	g++ main.cpp FTthreadManager.o -o main -lpthread -g

FTthreadManager.o: FTthreadManager.cxx FTthreadManager.h
	g++ -c FTthreadManager.cxx -g

clean: 
	rm -rf FTthreadManager.o main

.PHONY: clean
