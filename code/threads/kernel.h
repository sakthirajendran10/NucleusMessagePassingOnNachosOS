// kernel.h
//	Global variables for the Nachos kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef KERNEL_H
#define KERNEL_H

#include "copyright.h"
#include "debug.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "alarm.h"
#include "filesys.h"
#include "machine.h"
#include <map>
#include "list.h"
#include "bitmap.h"

class PostOfficeInput;
class PostOfficeOutput;
class SynchConsoleInput;
class SynchConsoleOutput;
class SynchDisk;

class Kernel {
  public:
    Kernel(int argc, char **argv);
    				// Interpret command line arguments
    ~Kernel();		        // deallocate the kernel
    //void NetworkServer(int which);
    void Initialize(); 		// initialize the kernel -- separated
				// from constructor because 
				// refers to "kernel" as a global

    void ThreadSelfTest();	// self test of threads and synchronization

    void ConsoleTest();         // interactive console self test

    void NetworkTest();         // interactive 2-machine network test
   
    int getSwapLocation();

    OpenFile* getSwapFile();
// These are public for notational convenience; really, 
// they're global variables used everywhere.

    Thread *currentThread;	// the thread holding the CPU
    Scheduler *scheduler;	// the ready list
    Interrupt *interrupt;	// interrupt status
    Statistics *stats;		// performance metrics
    Alarm *alarm;		// the software alarm clock    
    Machine *machine;           // the simulated CPU
    SynchConsoleInput *synchConsoleIn;
    SynchConsoleOutput *synchConsoleOut;
    SynchDisk *synchDisk;
    FileSystem *fileSystem;     
    PostOfficeInput *postOfficeIn;
    PostOfficeOutput *postOfficeOut;
    static int swapCounter;
    OpenFile *swapfileptr;
    int hostName;               // machine identifier
    int quantum=1000;
    List<int> *phyPageNos;
    map< int, Thread*> ppnToThreadMap;
    Bitmap *bmap;

//assignment3 changes
    Bitmap *bufferBitmap;
    map<string,Thread*> processMap;
    List<Thread*> *waitMessageThreads;
    List<Thread*> *waitAnswerThreads;
    //map<int,Port*> kernelPorts;

  private:
    bool randomSlice;		// enable pseudo-random time slicing
    bool debugUserProg;         // single step user program
    double reliability;         // likelihood messages are dropped
    char *consoleIn;            // file to read console input from
    char *consoleOut;           // file to send console output to
#ifndef FILESYS_STUB
    bool formatFlag;          // format the disk if this is true
#endif
};


#endif // KERNEL_H


